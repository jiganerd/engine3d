//
//  Graphics.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include <string>
#include <cmath>
#include <SDL2/SDL.h>
#include "Color.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec3Tex.hpp"
#include "Surface.hpp"

class Graphics
{
public:
    class SDLException
    {
    public:
        SDLException(std::string msg);
        std::string GetMsg() const;
    private:
        std::string error;
        std::string msg;
    };
    
    Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    void BeginFrame();
    void EndFrame();
    Surface LoadTexture(std::string filename);
    void PutPixel(int x, int y, int r, int g, int b);
    void PutPixel(int x, int y, const Color& c);
    void DrawLine(const Vec2& v1, const Vec2& v2, const Color& c);
    void DrawLine(float x1, float y1, float x2, float y2, const Color& c);
    void DrawTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c);
    void DrawTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex);
    ~Graphics();
    
private:
    // these follow triangle rasterization rules described at
    // https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules
    inline static int Rast(float n) { return static_cast<int>(ceil(n - 0.5f)); }
    void DrawFlatTopTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c);
    void DrawFlatBottomTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3, const Color& c);
    void DrawFlatTriangle(const Vec2& v1, const Vec2& v2, const Vec2& v3,
                          float xStepPerYLeft, float xStepPerYRight, const Vec2& upperRightV,
                          const Color& c);
    void DrawFlatTopTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex);
    void DrawFlatBottomTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3, const Surface& tex);
    void DrawFlatTriangleTex(const Vec3Tex& v1, const Vec3Tex& v2, const Vec3Tex& v3,
                             const Vec3Tex& stepPerYLeft, const Vec3Tex& stepPerYRight, const Vec3Tex& upperRightV,
                             const Surface& tex);
    
    Color GetSDLSurfaceColor(const SDL_Surface& surface, int x, int y);

    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;
    SDL_Texture* pScreenTexture;
    unsigned int* pPixelBuffer;
    
public:
    static constexpr unsigned int ScreenWidth = 640u;
    static constexpr unsigned int ScreenHeight = 640u;
};

#endif /* Graphics_hpp */
