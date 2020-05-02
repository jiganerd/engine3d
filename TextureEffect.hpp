//
//  TextureEffect.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/30/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef TextureEffect_hpp
#define TextureEffect_hpp

#include "Surface.hpp"
#include "Vec3.hpp"
#include "Vec2.hpp"
#include "Utils.hpp"

class TextureEffect
{
public:
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3& v, const Vec2& textureCoords):
            v(v),
        textureCoords(textureCoords)
        {
        }
        Vertex& operator=(const Vertex& rhs)
        {
            v = rhs.v;
            textureCoords = rhs.textureCoords;
            return *this;
        }
        Vertex operator+(const Vertex& rhs) const
        {
            return Vertex(v + rhs.v, textureCoords + rhs.textureCoords);
        }
        Vertex& operator+=(const Vertex& rhs)
        {
            *this = *this + rhs;
            return *this;
        }
        Vertex operator-(const Vertex& rhs) const
        {
            return Vertex(v - rhs.v, textureCoords - rhs.textureCoords);
        }
        Vertex& operator-=(const Vertex& rhs)
        {
            *this = *this - rhs;
            return *this;
        }
        Vertex operator*(float rhs) const
        {
            return Vertex(v * rhs, textureCoords * rhs);
        }
        Vertex& operator*=(float rhs)
        {
            *this = *this * rhs;
            return *this;
        }
        Vertex operator/(float rhs) const
        {
            return Vertex(v / rhs, textureCoords / rhs);
        }
        Vertex& operator/=(float rhs)
        {
            *this = *this / rhs;
            return *this;
        }
        Vertex InterpTo(const Vertex& rhs, float percent) const
        {
            return { v.InterpTo(rhs.v, percent),
                textureCoords.InterpTo(rhs.textureCoords, percent) };
        }
        
        Vec3 v;
        Vec2 textureCoords;
    };
    
    class PixelShader
    {
    public:
        PixelShader(Surface&& s):
            s(std::move(s))
        {}
        Color operator()(const Vertex& vertex)
        {
            // clamp to UV coordinates in case of floating point errors
            float u = vertex.textureCoords.x;
            float v = vertex.textureCoords.y;
            Utils::Clamp(u, 0.0f, 1.0f);
            Utils::Clamp(v, 0.0f, 1.0f);

            return s.GetPixelUV(u, v);
        };
       
    private:
        Surface s;
    };
    
    TextureEffect():
        pixelShader(Graphics::LoadTexture("brick.bmp"))
    {}
    
    PixelShader pixelShader;
};

#endif /* TextureEffect_hpp */
