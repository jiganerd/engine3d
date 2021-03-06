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
#include "SDLHeader.hpp"
#include "Color.hpp"
#include "Vec2.hpp"
#include "Surface.hpp"

class Graphics
{
public:
    class Exception
    {
    public:
        virtual std::string GetMsg() const = 0;
    };
    
    Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    void BeginFrame();
    void EndFrame();
    static Surface LoadTexture(std::string filename);
    void PutPixel(int x, int y, int r, int g, int b);
    void PutPixel(int x, int y, const Color& c);
    ~Graphics();
    
private:
    class SDLException : public Exception
    {
    public:
        SDLException(std::string msg);
        std::string GetMsg() const override;
    private:
        std::string error;
        std::string msg;
    };

    static Color GetSDLSurfaceColor(const SDL_Surface& surface, int x, int y);

    SDL_Window* pWindow;
    SDL_Renderer* pRenderer;
    SDL_Texture* pScreenTexture;
    Surface screen;
    
public:
    static constexpr unsigned int ScreenWidth = 640u;
    static constexpr unsigned int ScreenHeight = 640u;
};

#endif /* Graphics_hpp */
