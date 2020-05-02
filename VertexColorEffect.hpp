//
//  VertexColorEffect.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/30/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef VertexColorEffect_hpp
#define VertexColorEffect_hpp

#include "Vec3.hpp"
#include "Color.hpp"

class VertexColorEffect
{
public:
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3& v, const Vec3& c):
            v(v),
            c(c)
        {
        }
        Vertex& operator=(const Vertex& rhs)
        {
            v = rhs.v;
            c = rhs.c;
            return *this;
        }
        Vertex operator+(const Vertex& rhs) const
        {
            return Vertex(v + rhs.v, c + rhs.c);
        }
        Vertex& operator+=(const Vertex& rhs)
        {
            *this = *this + rhs;
            return *this;
        }
        Vertex operator-(const Vertex& rhs) const
        {
            return Vertex(v - rhs.v, c - rhs.c);
        }
        Vertex& operator-=(const Vertex& rhs)
        {
            *this = *this - rhs;
            return *this;
        }
        Vertex operator*(float rhs) const
        {
            return Vertex(v * rhs, c * rhs);
        }
        Vertex& operator*=(float rhs)
        {
            *this = *this * rhs;
            return *this;
        }
        Vertex operator/(float rhs) const
        {
            return Vertex(v / rhs, c / rhs);
        }
        Vertex& operator/=(float rhs)
        {
            *this = *this / rhs;
            return *this;
        }
        Vertex InterpTo(const Vertex& rhs, float percent) const
        {
            return { v.InterpTo(rhs.v, percent),
                c.InterpTo(rhs.c, percent) };
        }
        
        Vec3 v;
        Vec3 c; // color
    };
    
    class PixelShader
    {
    public:
        Color operator()(const Vertex& vertex)
        {
            return Color(static_cast<char>(vertex.c.x),
                         static_cast<char>(vertex.c.y),
                         static_cast<char>(vertex.c.z));
        };
        
    private:
    };
    
    PixelShader pixelShader;
};

#endif /* VertexColorEffect_hpp */
