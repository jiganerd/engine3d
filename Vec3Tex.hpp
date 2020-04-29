//
//  Vec3Tex.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/27/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Vec3Tex_hpp
#define Vec3Tex_hpp

#include "Vec3.hpp"
#include "Vec2.hpp"

class Vec3Tex
{
public:
    Vec3Tex() = default;
    Vec3Tex(const Vec3& v, const Vec2& textureCoords):
        v(v),
        textureCoords(textureCoords)
    {
    }
    Vec3Tex& operator=(const Vec3Tex& rhs)
    {
        v = rhs.v;
        textureCoords = rhs.textureCoords;
        return *this;
    }
    Vec3Tex operator+(const Vec3Tex& rhs) const
    {
        return Vec3Tex(v + rhs.v, textureCoords + rhs.textureCoords);
    }
    Vec3Tex& operator+=(const Vec3Tex& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    Vec3Tex operator-(const Vec3Tex& rhs) const
    {
        return Vec3Tex(v - rhs.v, textureCoords - rhs.textureCoords);
    }
    Vec3Tex& operator-=(const Vec3Tex& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    Vec3Tex operator*(float rhs) const
    {
        return Vec3Tex(v * rhs, textureCoords * rhs);
    }
    Vec3Tex& operator*=(float rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    Vec3Tex operator/(float rhs) const
    {
        return Vec3Tex(v / rhs, textureCoords / rhs);
    }
    Vec3Tex& operator/=(float rhs)
    {
        *this = *this / rhs;
        return *this;
    }
    Vec3Tex InterpTo(const Vec3Tex& rhs, float percent) const
    {
        return { v.InterpTo(rhs.v, percent),
            textureCoords.InterpTo(rhs.textureCoords, percent) };
    }
    
    Vec3 v;
    Vec2 textureCoords;
};

#endif /* Vec3Tex_hpp */
