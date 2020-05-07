//
//  GouraudEffect.hpp
//  engine3d
//
//  Created by Brian Dolan on 5/6/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef GouraudEffect_hpp
#define GouraudEffect_hpp

#include "Vec3.hpp"
#include "Color.hpp"
#include "Mat3.hpp"
#include "Triangle.hpp"

// vertices are lit according to mesh-defined normals, and colors are interpolated between them
// screen-linearly
class GouraudEffect
{
public:
    // includes an attribute for the normal, but which is not interpolated
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3& v, const Vec3& norm):
            v(v),
            norm(norm)
        {
        }
        Vertex& operator=(const Vertex& rhs)
        {
            v = rhs.v;
            return *this;
        }
        Vertex operator+(const Vertex& rhs) const
        {
            return Vertex(v + rhs.v, norm);
        }
        Vertex& operator+=(const Vertex& rhs)
        {
            *this = *this + rhs;
            return *this;
        }
        Vertex operator-(const Vertex& rhs) const
        {
            return Vertex(v - rhs.v, norm);
        }
        Vertex& operator-=(const Vertex& rhs)
        {
            *this = *this - rhs;
            return *this;
        }
        Vertex operator*(float rhs) const
        {
            return Vertex(v * rhs, norm);
        }
        Vertex& operator*=(float rhs)
        {
            *this = *this * rhs;
            return *this;
        }
        Vertex operator/(float rhs) const
        {
            return Vertex(v / rhs, norm);
        }
        Vertex& operator/=(float rhs)
        {
            *this = *this / rhs;
            return *this;
        }
        Vertex InterpTo(const Vertex& rhs, float percent) const
        {
            return { v.InterpTo(rhs.v, percent), norm };
        }
        
        Vec3 v;
        Vec3 norm;
    };
    
    // handles rotation/translation
    // outputs color at each vertex, as per normals/lighting
    class VertexShader
    {
    public:
        // position and color, with interpolation for both
        class OutVertex
        {
        public:
            OutVertex() = default;
            OutVertex(const Vec3& v, const Vec3& c):
                v(v),
                c(c)
            {
            }
            OutVertex& operator=(const OutVertex& rhs)
            {
                v = rhs.v;
                c = rhs.c;
                return *this;
            }
            OutVertex operator+(const OutVertex& rhs) const
            {
                return OutVertex(v + rhs.v, c + rhs.c);
            }
            OutVertex& operator+=(const OutVertex& rhs)
            {
                *this = *this + rhs;
                return *this;
            }
            OutVertex operator-(const OutVertex& rhs) const
            {
                return OutVertex(v - rhs.v, c - rhs.c);
            }
            OutVertex& operator-=(const OutVertex& rhs)
            {
                *this = *this - rhs;
                return *this;
            }
            OutVertex operator*(float rhs) const
            {
                return OutVertex(v * rhs, c * rhs);
            }
            OutVertex& operator*=(float rhs)
            {
                *this = *this * rhs;
                return *this;
            }
            OutVertex operator/(float rhs) const
            {
                return OutVertex(v / rhs, c / rhs);
            }
            OutVertex& operator/=(float rhs)
            {
                *this = *this / rhs;
                return *this;
            }
            OutVertex InterpTo(const OutVertex& rhs, float percent) const
            {
                return { v.InterpTo(rhs.v, percent),
                    c.InterpTo(rhs.c, percent) };
            }
            
            Vec3 v;
            Vec3 c; // color
        };
        
        VertexShader():
            lightDir(Vec3(1.0f, -1.0f, 2.0f).Norm()),
            ambientLight(0.2f),
            vc(Color(Colors::White).Vec())
        {}
        OutVertex operator()(const Vertex& vertex)
        {
            // rotate the normal, but don't translate it!
            Vec3 rotNorm = vertex.norm * rotMat;
            
            // shade according to lighting
            float intensity = std::max(-(rotNorm * lightDir), ambientLight);
            
            // rotate and translate the position vector
            return OutVertex(vertex.v * rotMat + transVec, vc * intensity);
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
        const Vec3 lightDir;
        const float ambientLight;
        Vec3 vc; // color
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
        Color operator()(const GeometryShader::OutVertex& gsOutVertex)
        {
            // simply return the color, which was linearly interpolated between vertices
            return gsOutVertex.c;
        };
        
    private:
    };
    
    VertexShader vertexShader;
    GeometryShader geometryShader;
    PixelShader pixelShader;
};

#endif /* GouraudEffect_hpp */
