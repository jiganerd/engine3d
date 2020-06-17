//
//  Color.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Color_hpp
#define Color_hpp

#include "Vec3.hpp"

class Color
{
public:
    constexpr Color(unsigned char r, unsigned char g, unsigned char b):
        argb(0x00000000 |
             static_cast<unsigned int>(r) << 16 |
             static_cast<unsigned int>(g) << 8 |
             static_cast<unsigned int>(b))
    { }
    constexpr Color(unsigned int argb):
        argb(argb)
    { }
    constexpr Color(const Vec3& v):
        argb(0x00000000 |
             static_cast<unsigned int>(v.x) << 16 |
             static_cast<unsigned int>(v.y) << 8 |
             static_cast<unsigned int>(v.z))
    { }
    constexpr operator unsigned int() const
    {
        return argb;
    }
    constexpr Color operator*(float n) const
    {
        return Color(
			static_cast<unsigned char>(R()*n), 
			static_cast<unsigned char>(G()*n), 
			static_cast<unsigned char>(B()*n));
    }
    Color& operator*=(float n)
    {
        *this = *this * n;
        return *this;
    }
    void SetRGB(unsigned char r, unsigned char g, unsigned char b)
    {
        argb = 0x00000000 |
                static_cast<unsigned int>(r) << 16 |
                static_cast<unsigned int>(g) << 8 |
                static_cast<unsigned int>(b);
    }
    void SetRGB(const Vec3& v)
    {
        SetRGB(static_cast<unsigned char>(v.x),
               static_cast<unsigned char>(v.y),
               static_cast<unsigned char>(v.z));
    }
    constexpr unsigned char R() const
    {
        return ((argb & 0x00FF0000) >> 16);
    }
    constexpr unsigned char G() const
    {
        return ((argb & 0x0000FF00) >> 8);
    }
    constexpr unsigned char B() const
    {
        return ((argb & 0x000000FF));
    }
    Vec3 Vec()
    {
        return Vec3(static_cast<float>(R()),
                    static_cast<float>(G()),
                    static_cast<float>(B()));
    }
    void SetR(unsigned char r)
    {
        argb = ((argb & 0xFF00FFFF) | (static_cast<unsigned int>(r) << 16));
    }
    void SetG(unsigned char g)
    {
        argb = ((argb & 0xFFFF00FF) | (static_cast<unsigned int>(g) << 8));
    }
    void SetB(unsigned char b)
    {
        argb = ((argb & 0xFFFFFF00) | static_cast<unsigned int>(b));
    }
public:
    unsigned int argb;
};

// based on standard HTML colors
namespace Colors
{
    static constexpr Color Black   = Color(  0,   0,   0);
    static constexpr Color White   = Color(255, 255, 255);
    
    static constexpr Color Red     = Color(255,   0,   0);
    static constexpr Color Lime    = Color(  0, 255,   0);
    static constexpr Color Blue    = Color(  0,   0, 255);
    
    static constexpr Color Yellow  = Color(255, 255,   0);
    static constexpr Color Cyan    = Color(  0, 255, 255);
    static constexpr Color Magenta = Color(255,   0, 255);

    static constexpr Color Silver  = Color(192, 192, 192);
    static constexpr Color Gray    = Color(128, 128, 128);
    static constexpr Color Maroon  = Color(128,   0,   0);
    static constexpr Color Olive   = Color(128, 128,   0);
    static constexpr Color Green   = Color(  0, 128,   0);
    static constexpr Color Purple  = Color(128,   0, 128);
    static constexpr Color Teal    = Color(  0, 128, 128);
    static constexpr Color Navy    = Color(  0,   0, 128);
}

#endif /* Color_hpp */
