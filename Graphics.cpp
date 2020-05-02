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
    screen(ScreenWidth, ScreenHeight)
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
    memset(screen.GetPixelBuffer(), 0, ScreenWidth * ScreenHeight * sizeof(uint32_t)/sizeof(uint8_t));
}

void Graphics::EndFrame()
{
    if (SDL_UpdateTexture(pScreenTexture, NULL, screen.GetPixelBuffer(), ScreenWidth * sizeof(unsigned int)) < 0)
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
    screen.PutPixel(x, y, c);
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