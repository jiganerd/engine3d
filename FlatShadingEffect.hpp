//
//  FlatShadingEffect.hpp
//  engine3d
//
//  Created by Brian Dolan on 5/3/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef FlatShadingEffect_hpp
#define FlatShadingEffect_hpp

#include "Vec3.hpp"
#include "Color.hpp"

// entire triangles are lit according to their plane normals
class FlatShadingEffect
{
public:
    // contains only a position vector
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3& v):
            v(v)
        {
        }
        Vertex& operator=(const Vertex& rhs)
        {
            v = rhs.v;
            return *this;
        }
        Vertex operator+(const Vertex& rhs) const
        {
            return Vertex(v + rhs.v);
        }
        Vertex& operator+=(const Vertex& rhs)
        {
            *this = *this + rhs;
            return *this;
        }
        Vertex operator-(const Vertex& rhs) const
        {
            return Vertex(v - rhs.v);
        }
        Vertex& operator-=(const Vertex& rhs)
        {
            *this = *this - rhs;
            return *this;
        }
        Vertex operator*(float rhs) const
        {
            return Vertex(v * rhs);
        }
        Vertex& operator*=(float rhs)
        {
            *this = *this * rhs;
            return *this;
        }
        Vertex operator/(float rhs) const
        {
            return Vertex(v / rhs);
        }
        Vertex& operator/=(float rhs)
        {
            *this = *this / rhs;
            return *this;
        }
        Vertex InterpTo(const Vertex& rhs, float percent) const
        {
            return { v.InterpTo(rhs.v, percent) };
        }
        
        Vec3 v;
    };
    
    // only handles rotation and translation
    class VertexShader
    {
    public:
        typedef Vertex OutVertex;
        
        OutVertex operator()(const Vertex& vertex)
        {
            return OutVertex(vertex.v * rotMat + transVec);
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
 
    // for each triangle, calculate the normal for the plane and add it on each vertex
    // (information duplicated)
    // (these normals are not interpolated)
    class GeometryShader
    {
    public:
        class OutVertex
        {
        public:
            OutVertex() = default;
            OutVertex(const Vec3& v, const Vec3& norm):
                v(v),
                norm(norm)
            {
            }
            OutVertex& operator=(const OutVertex& rhs)
            {
                v = rhs.v;
                norm = rhs.norm;
                return *this;
            }
            OutVertex operator+(const OutVertex& rhs) const
            {
                return OutVertex(v + rhs.v, norm);
            }
            OutVertex& operator+=(const OutVertex& rhs)
            {
                *this = *this + rhs;
                return *this;
            }
            OutVertex operator-(const OutVertex& rhs) const
            {
                return OutVertex(v - rhs.v, norm);
            }
            OutVertex& operator-=(const OutVertex& rhs)
            {
                *this = *this - rhs;
                return *this;
            }
            OutVertex operator*(float rhs) const
            {
                return OutVertex(v * rhs, norm);
            }
            OutVertex& operator*=(float rhs)
            {
                *this = *this * rhs;
                return *this;
            }
            OutVertex operator/(float rhs) const
            {
                return OutVertex(v / rhs, norm);
            }
            OutVertex& operator/=(float rhs)
            {
                *this = *this / rhs;
                return *this;
            }
            OutVertex InterpTo(const OutVertex& rhs, float percent) const
            {
                return { v.InterpTo(rhs.v, percent), norm };
            }
            
            Vec3 v;
            Vec3 norm;
        };
        
        Triangle<GeometryShader::OutVertex> operator()(const Triangle<VertexShader::OutVertex>& t)
        {
            // calculate a single normal for the entire triangle and attach it to all 3 vertices
            Vec3 norm = (t.v2.v - t.v1.v).cross(t.v3.v - t.v1.v).Norm();
            return Triangle<GeometryShader::OutVertex>({{t.v1.v, norm},
                {t.v2.v, norm},
                {t.v3.v, norm}});
        }
    };

    class PixelShader
    {
    public:
        PixelShader():
            lightDir(Vec3(1.0f, -1.0f, 2.0f).Norm()),
            ambientLight(0.2f),
            c(Colors::White)
        {}
        Color operator()(const GeometryShader::OutVertex& gsOutVertex)
        {
            // shade according to lighting
            float intensity = std::max(-(gsOutVertex.norm * lightDir), ambientLight);
            return (c * intensity);
        };
        
    private:
        const Vec3 lightDir;
        const float ambientLight;
        Color c;
    };
    
    VertexShader vertexShader;
    GeometryShader geometryShader;
    PixelShader pixelShader;
};

#endif /* FlatShadingEffect_hpp */
