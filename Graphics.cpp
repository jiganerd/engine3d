//
//  Graphics.cpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <assert.h>
#include <algorithm>
#include "Graphics.hpp"
#include "Utils.hpp"

Graphics::SDLException::SDLException(std::string msg):
    error(SDL_GetError()),
    msg(msg)
{
}

std::string Graphics::SDLException::GetMsg() const
{
    return "SDLException: " + msg + ": " + error;
}

Graphics::Graphics() :
    pPixelBuffer(new unsigned int[ScreenWidth * ScreenHeight])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLException("Error initializating SDL");
    
    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    SDL_CreateWindowAndRenderer(ScreenWidth, ScreenHeight, windowFlags, &pWindow, &pRenderer);
    if (pWindow == NULL)
        throw SDLException("Window could not be created");

    pScreenTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, ScreenWidth, ScreenHeight);
    if (pScreenTexture == NULL)
        throw SDLException("Could not create screen texture");
    
    SDL_RaiseWindow(pWindow);
}

Graphics::~Graphics()
{
    SDL_DestroyTexture(pScreenTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

void Graphics::BeginFrame()
{
    memset(pPixelBuffer, 0, ScreenWidth * ScreenHeight * sizeof(uint32_t)/sizeof(uint8_t));
}

void Graphics::EndFrame()
{
    if (SDL_UpdateTexture(pScreenTexture, NULL, pPixelBuffer, ScreenWidth * sizeof(unsigned int)) < 0)
        throw SDLException("Could not update screen texture");
    
    if (SDL_RenderCopy(pRenderer, pScreenTexture, NULL, NULL) < 0)
        throw SDLException("Could not render screen copy");
    
    SDL_RenderPresent(pRenderer);
}

Surface Graphics::LoadTexture(std::string filename)
{
    SDL_Surface* pSurf = SDL_LoadBMP(filename.c_str());
    if (!pSurf)
        throw SDLException("Could not load texture");
   
    Surface s(pSurf->w, pSurf->h);
    
    // load image into pixels array in ARGB format
    // (no problem if this is slow)
    auto pPixelBuffer = s.GetPixelBuffer();
    int offset = 0;
    for (int y = 0; y < pSurf->h; y++)
    {
        for (int x = 0; x < pSurf->w; x++)
        {
            pPixelBuffer[offset++] = GetSDLSurfaceColor(*pSurf, x, y);
        }
    }
    return s;
}

void Graphics::PutPixel(int x, int y, int r, int g, int b)
{
    PutPixel(x, y, Color(r, g, b));
}

void Graphics::PutPixel(int x, int y, const Color& c)
{
    assert(x >= 0);
    assert(y >= 0);
    assert(x < ScreenWidth);
    assert(y < ScreenHeight);
    pPixelBuffer[y * ScreenWidth + x] = c;
}

void Graphics::DrawLine(const Vec2& v1, const Vec2& v2, const Color& c)
{
    DrawLine(v1.x, v1.y, v2.x, v2.y, c);
}

// taken from CS480 homework!
// it is not perfect - it does not line up perfectly with the rasterization rules in
// the triangle rasterization functions (which *are* perfect!)
void Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& c)
{
    // if we've been passed a single point, don't bother drawing a line
    if (x1 == x2 && y1 == y2)
    {
        PutPixel(Rast(x1), Rast(y1), c);
    }
    else
    {
        int i;
        
        float x = x1, y = y1;
        
        int numPix; // the number of times to iterate
        int distX = Rast(x2) - Rast(x1); // the dimensions of the line
        int distY = Rast(y2) - Rast(y1);
        
        // should we scan horizontally or vertically?
        if (abs(distX) > abs(distY)) numPix = abs(distX);
        else numPix = abs(distY);
        
        // calculate the incremental x and y values (can be pos. or neg.)
        float deltaX = static_cast<float>(distX) / static_cast<float>(numPix);
        float deltaY = static_cast<float>(distY) / static_cast<float>(numPix);
        
        // draw the line
        for (i = 0; i < numPix; i++)
        {
            PutPixel(Rast(x), Rast(y), c);
            x += deltaX;
            y += deltaY;
        }
        PutPixel(Rast(x), Rast(y), c);
    }
}

void Graphics::DrawTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c)
{
    // rearrange vertices such that v1 is at the top and v3 is at the bottom
    const Vec2* pV1 = &v1;
    const Vec2* pV2 = &v2;
    const Vec2* pV3 = &v3;
    
    if (pV2->y < pV1->y)
        std::swap(pV1, pV2);
    
    if (pV3->y < pV2->y)
    {
        std::swap(pV2, pV3);
        if (pV2->y < pV1->y)
            std::swap(pV1, pV2);
    }
    
    if (pV1->y == pV2->y)
    {
        if (pV2->x < pV1->x)
            std::swap(pV1, pV2);
        DrawFlatTopTriangle(*pV1, *pV2, *pV3, c);
    }
    else if (pV2->y == pV3->y)
    {
        if (pV3->x < pV2->x)
            std::swap(pV2, pV3);
        DrawFlatBottomTriangle(*pV1, *pV2, *pV3, c);
    }
    else
    {
        float heightRatio = (pV2->y - pV1->y)/(pV3->y - pV1->y);
        Vec2 vSplit = (*pV1) + (*pV3 - *pV1) * heightRatio;
        
        if (vSplit.x < pV2->x)
        {
            DrawFlatBottomTriangle(*pV1, vSplit, *pV2, c);
            DrawFlatTopTriangle(vSplit, *pV2, *pV3, c);
        }
        else
        {
            DrawFlatBottomTriangle(*pV1, *pV2, vSplit, c);
            DrawFlatTopTriangle(*pV2, vSplit, *pV3, c);
        }
    }
}

void Graphics::DrawTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex)
{
    // rearrange vertices such that v1 is at the top and v3 is at the bottom
    const Vec3Tex* pV1 = &v1;
    const Vec3Tex* pV2 = &v2;
    const Vec3Tex* pV3 = &v3;
    
    if (pV2->v.y < pV1->v.y)
        std::swap(pV1, pV2);
    
    if (pV3->v.y < pV2->v.y)
    {
        std::swap(pV2, pV3);
        if (pV2->v.y < pV1->v.y)
            std::swap(pV1, pV2);
    }
    
    if (pV1->v.y == pV2->v.y)
    {
        if (pV2->v.x < pV1->v.x)
            std::swap(pV1, pV2);
        DrawFlatTopTriangleTex(*pV1, *pV2, *pV3, tex);
    }
    else if (pV2->v.y == pV3->v.y)
    {
        if (pV3->v.x < pV2->v.x)
            std::swap(pV2, pV3);
        DrawFlatBottomTriangleTex(*pV1, *pV2, *pV3, tex);
    }
    else
    {
        float heightRatio = (pV2->v.y - pV1->v.y)/(pV3->v.y - pV1->v.y);
        Vec3Tex vSplit = pV1->InterpTo(*pV3, heightRatio);
        
        if (vSplit.v.x < pV2->v.x)
        {
            DrawFlatBottomTriangleTex(*pV1, vSplit, *pV2, tex);
            DrawFlatTopTriangleTex(vSplit, *pV2, *pV3, tex);
        }
        else
        {
            DrawFlatBottomTriangleTex(*pV1, *pV2, vSplit, tex);
            DrawFlatTopTriangleTex(*pV2, vSplit, *pV3, tex);
        }
    }
}

//   v1 *-------* v2
//       \     /
//        \   /
//         \ /
//          * v3
void Graphics::DrawFlatTopTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c)
{
    // for both the left side and right side of the triangle, calculate the (floating point) step
    // in the x direction that should be taken for each y scanline step
    float triangleHeight = v3.y - v1.y;
    float stepPerYLeft = (v3.x - v1.x) / triangleHeight;
    float stepPerYRight = (v3.x - v2.x) / triangleHeight;
    
    DrawFlatTriangle(v1, v2, v3, stepPerYLeft, stepPerYRight, v2, c);
}

//          * v1
//         / \
//        /   \
//       /     \
//   v2 *-------* v3
void Graphics::DrawFlatBottomTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c)
{
    // for both the left side and right side of the triangle, calculate the (floating point) step
    // in the x direction that should be taken for each y scanline step
    float triangleHeight = v3.y - v1.y;
    float stepPerYLeft = (v2.x - v1.x) / triangleHeight;
    float stepPerYRight = (v3.x - v1.x) / triangleHeight;
    
    DrawFlatTriangle(v1, v2, v3, stepPerYLeft, stepPerYRight, v1, c);
}

void Graphics::DrawFlatTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3,
                                float xStepPerYLeft, float xStepPerYRight, const Vec2& upperRightV,
                                const Color& c)
{
    int yStart = Rast(v1.y);
    int yEnd = Rast(v3.y);
    
    // *rough* initial values for starting/ending x, but...
    float xStart = v1.x;
    float xEnd = upperRightV.x;
    
    // this logic may not be intuitive - we need to "bump" the x start and end values a bit:
    // we actually should be calculating the x values based on the y value for the *vertical center*
    // of the first row of pixels (not the upper edge)
    float yOffsetForFirstRow = static_cast<float>(yStart) + 0.5f - v1.y;
    xStart += yOffsetForFirstRow * xStepPerYLeft;
    xEnd   += yOffsetForFirstRow * xStepPerYRight;
    
    for (int y = yStart; y < yEnd; y++)
    {
        int xStartI = Rast(xStart);
        int xEndI = Rast(xEnd);
        
        for (int x = xStartI; x < xEndI; x++)
        {
            PutPixel(x, y, c);
        }
        
        xStart += xStepPerYLeft;
        xEnd += xStepPerYRight;
    }
}

//   v1 *-------* v2
//       \     /
//        \   /
//         \ /
//          * v3
void Graphics::DrawFlatTopTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex)
{
    // for both the left side and right side of the triangle, calculate the (floating point) step
    // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
    // should be taken for each y scanline step
    float triangleHeight = v3.v.y - v1.v.y;
    Vec3Tex stepPerYLeft = (v3 - v1) / triangleHeight;
    Vec3Tex stepPerYRight = (v3 - v2) / triangleHeight;

    DrawFlatTriangleTex(v1, v2, v3, stepPerYLeft, stepPerYRight, v2, tex);
}

//          * v1
//         / \
//        /   \
//       /     \
//   v2 *-------* v3
void Graphics::DrawFlatBottomTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex)
{
    // for both the left side and right side of the triangle, calculate the (floating point) step
    // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
    // should be taken for each y scanline step
    float triangleHeight = v3.v.y - v1.v.y;
    Vec3Tex stepPerYLeft = (v2 - v1) / triangleHeight;
    Vec3Tex stepPerYRight = (v3 - v1) / triangleHeight;
    
    DrawFlatTriangleTex(v1, v2, v3, stepPerYLeft, stepPerYRight, v1, tex);
}

void Graphics::DrawFlatTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3,
                                   const Vec3Tex& stepPerYLeft, const Vec3Tex& stepPerYRight, const Vec3Tex& upperRightV,
                                   const Surface& tex)
{
    // quantize the beginning (inclusive) and end (non-inclusive) y values for the top and bottom of
    // the triangle, following our rasterization rules
    int yStart = Rast(v1.v.y);
    int yEnd = Rast(v3.v.y);
    
    // *rough* initial values for starting/ending x, but...
    Vec3Tex xStart = v1; // (same for either flat top triangle or flat bottom triangle)
    Vec3Tex xEnd = upperRightV;
    
    // this logic may not be intuitive - we need to "bump" the x start and end values a bit:
    // we actually should be calculating the x values based on the y value for the *vertical center*
    // of the first row of pixels (not the upper edge)
    float yOffsetForFirstRow = static_cast<float>(yStart) + 0.5f - v1.v.y;
    xStart += stepPerYLeft * yOffsetForFirstRow;
    xEnd += stepPerYRight * yOffsetForFirstRow;
    
    Vec2 textureCoords;
    Vec2 textureCoordsStepPerX;
    
    for (int y = yStart; y < yEnd; y++)
    {
        // quantize the beginning (inclusive) and end (non-inclusive) x values for the left and right of
        // the triangle, following our rasterization rules
        int xStartI = Rast(xStart.v.x);
        int xEndI = Rast(xEnd.v.x);
        
        textureCoordsStepPerX = (xEnd.textureCoords - xStart.textureCoords) / (xEnd.v.x - xStart.v.x);
        
        // *rough* initial values for starting x, but...
        textureCoords = xStart.textureCoords;
        
        // similar to above, we will "bump" the texture starting coordinates a bit based on
        // the horizontal center of the first column
        textureCoords += textureCoordsStepPerX * (static_cast<float>(xStartI) + 0.5f - xStart.v.x);
        
        for (int x = xStartI; x < xEndI; x++)
        {
            // clamp to UV coordinates in case of floating point errors
            Utils::Clamp(textureCoords.x, 0.0f, 1.0f);
            Utils::Clamp(textureCoords.y, 0.0f, 1.0f);
            
            PutPixel(x, y, tex.GetPixelUV(textureCoords.x, textureCoords.y));
            
            textureCoords += textureCoordsStepPerX;
        }
        
        xStart += stepPerYLeft;
        xEnd += stepPerYRight;
    }
}

Color Graphics::GetSDLSurfaceColor(const SDL_Surface& surface, int x, int y)
{
    const SDL_PixelFormat* pPixelFormat = surface.format;
    unsigned char *pPixels = (unsigned char *)surface.pixels;
    
    unsigned char* pPixel = &pPixels[((y * surface.w) + x) * pPixelFormat->BytesPerPixel];
    
    Uint32 pixel;
    switch (pPixelFormat->BytesPerPixel)
    {
        case 1:
            pixel = *pPixel;
            break;
            
        case 2:
            pixel = *(Uint16 *)pPixel;
            break;
            
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                pixel = pPixel[0] << 16 | pPixel[1] << 8 | pPixel[2];
            else
                pixel = pPixel[0] | pPixel[1] << 8 | pPixel[2] << 16;
            break;
            
        case 4:
            pixel = *(Uint32 *)pPixel;
            break;
            
        default:
            pixel = 0;
    }

    Uint8 r, g, b;
    SDL_GetRGB(pixel, pPixelFormat, &r, &g, &b);
    return Color(r, g, b);
}