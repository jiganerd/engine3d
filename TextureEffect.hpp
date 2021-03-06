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
#include "Mat3.hpp"
#include "Utils.hpp"
#include "Triangle.hpp"

// textures triangles
// entire triangles are lit according to their plane normals
class TextureEffect
{
public:
    // contains an (interpolated) attribute for texture coordinates
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

    // only handles rotation and translation
    class VertexShader
    {
    public:
        typedef Vertex OutVertex;
        
        OutVertex operator()(const Vertex& vertex)
        {
            return OutVertex(vertex.v * rotMat + transVec, vertex.textureCoords);
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
    
    // for each triangle, calculate the light intensity based on the normal for the plane,
    // and duplicate it on each vertex
    // (and do not interpolate intensity between vertices)
    class GeometryShader
    {
    public:
        // contains texture coordinates and light intensity (not interpolated)
        class OutVertex
        {
        public:
            OutVertex() = default;
            OutVertex(const Vec3& v, const Vec2& textureCoords, float intensity):
                v(v),
                textureCoords(textureCoords),
                intensity(intensity)
            {}
            OutVertex& operator=(const OutVertex& rhs)
            {
                v = rhs.v;
                textureCoords = rhs.textureCoords;
                intensity = rhs.intensity;
                return *this;
            }
            OutVertex operator+(const OutVertex& rhs) const
            {
                return OutVertex(v + rhs.v, textureCoords + rhs.textureCoords, intensity);
            }
            OutVertex& operator+=(const OutVertex& rhs)
            {
                *this = *this + rhs;
                return *this;
            }
            OutVertex operator-(const OutVertex& rhs) const
            {
                return OutVertex(v - rhs.v, textureCoords - rhs.textureCoords, intensity);
            }
            OutVertex& operator-=(const OutVertex& rhs)
            {
                *this = *this - rhs;
                return *this;
            }
            OutVertex operator*(float rhs) const
            {
                return OutVertex(v * rhs, textureCoords * rhs, intensity);
            }
            OutVertex& operator*=(float rhs)
            {
                *this = *this * rhs;
                return *this;
            }
            OutVertex operator/(float rhs) const
            {
                return OutVertex(v / rhs, textureCoords / rhs, intensity);
            }
            OutVertex& operator/=(float rhs)
            {
                *this = *this / rhs;
                return *this;
            }
            OutVertex InterpTo(const OutVertex& rhs, float percent) const
            {
                return { v.InterpTo(rhs.v, percent),
                    textureCoords.InterpTo(rhs.textureCoords, percent),
                    intensity };
            }
 
            Vec3 v;
            Vec2 textureCoords;
            float intensity;
        };
        
        GeometryShader():
            lightDir(Vec3(1.0f, -1.0f, 2.0f).Norm()),
            ambientLight(0.2f)
        {}
        
        Triangle<GeometryShader::OutVertex> operator()(const Triangle<VertexShader::OutVertex>& t)
        {
            // calculate a single normal for the entire triangle and attach it to all 3 vertices
            Vec3 norm = (t.v2.v - t.v1.v).cross(t.v3.v - t.v1.v).Norm();
            
            // figure out light intensity based on normal
            float intensity = std::max(-(norm * lightDir), ambientLight);
            
            return Triangle<GeometryShader::OutVertex>({{t.v1.v, t.v1.textureCoords, intensity},
                                                        {t.v2.v, t.v2.textureCoords, intensity},
                                                        {t.v3.v, t.v3.textureCoords, intensity}});
        }
        
    private:
        const Vec3 lightDir;
        const float ambientLight;
    };

    // calculate pixel color based on light intensity and texture color
    class PixelShader
    {
    public:
        PixelShader(Surface&& s):
            s(std::move(s))
        {}
        Color operator()(const GeometryShader::OutVertex& gsOutVertex)
        {
            // clamp to UV coordinates in case of floating point errors
            float u = gsOutVertex.textureCoords.x;
            float v = gsOutVertex.textureCoords.y;
            Utils::Clamp(u, 0.0f, 1.0f);
            Utils::Clamp(v, 0.0f, 1.0f);

            Color c = s.GetPixelUV(u, v);

            // shade according to light intensity
            c *= gsOutVertex.intensity;

            return c;
        };
       
    private:
        Surface s;
    };
    
    TextureEffect():
        pixelShader(Graphics::LoadTexture("brick.bmp"))
    {}
    
    VertexShader vertexShader;
    GeometryShader geometryShader;
    PixelShader pixelShader;
};

#endif /* TextureEffect_hpp */
