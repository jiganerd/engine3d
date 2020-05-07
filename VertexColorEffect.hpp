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

// colors are assigned to vertices and interpolated between them screen-linearly
// no lighting effects
class VertexColorEffect
{
public:
    // includes an attribute for color, which is interpolated
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
    
    // only handles rotation and translation
    class VertexShader
    {
    public:
        typedef Vertex OutVertex;
        
        OutVertex operator()(const Vertex& vertex)
        {
            return OutVertex(vertex.v * rotMat + transVec, vertex.c);
        };
        void BindRotation(const Mat3& rotMat)
        {
            this->rotMat = rotMat;
        }
        void BindTranslation(const Vec3& transVec)
        {
            this->transVec = transVec;
        }
        
    private:
        Mat3 rotMat;
        Vec3 transVec;
    };

    // a dumb "pass-through" geometry shader - the input and output types are the same type
    class GeometryShader
    {
    public:
        typedef VertexShader::OutVertex OutVertex;
        
        Triangle<GeometryShader::OutVertex> operator()(const Triangle<VertexShader::OutVertex>& t)
        {
            return t;
        }
    };

    class PixelShader
    {
    public:
        Color operator()(const Vertex& vertex)
        {
            return Color(vertex.c);
        };
        
    private:
    };
    
    VertexShader vertexShader;
    GeometryShader geometryShader;
    PixelShader pixelShader;
};

#endif /* VertexColorEffect_hpp */
