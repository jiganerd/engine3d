//
//  Pipeline.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/30/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Pipeline_hpp
#define Pipeline_hpp

#include <vector>
#include "Color.hpp"
#include "Surface.hpp"
#include "Vec2.hpp"
#include "Mat3.hpp"
#include "Graphics.hpp"
#include "ScreenTransform.hpp"
#include "IndexedLineList.hpp"
#include "IndexedTriangleList.hpp"
#include "Utils.hpp"

template <typename Effect>
class Pipeline
{
    using Vertex = typename Effect::Vertex;
    using PixelShader = typename Effect::PixelShader;
    
public:
    Pipeline(Graphics& g):
        g(g)
    {}
    void Draw(const IndexedTriangleList<Vertex>& itl)
    {
        const std::vector<Color> colors =
        {
            Colors::Red,
            Colors::Lime,
            Colors::Blue,
            Colors::Yellow,
            Colors::Cyan,
            Colors::Magenta,
            Colors::Silver,
            Colors::Gray,
            Colors::Maroon,
            Colors::Olive,
            Colors::Green,
            Colors::Purple
        };
        
        static float angle = 0.1;
        angle += 0.002;
        
        Mat3 rotMat = Mat3::RotZ(angle) * Mat3::RotX(angle*2) * Mat3::RotY(angle/2);
        Vec3 transVec(0.0f, 0.0f, 2.0f);
        
        // translate everything in world space
        std::vector<Vertex> transformedVertices;
        for (auto& v : itl.vertices)
        {
            // put the transformed 3-dimensional vector into the new vertex while
            // ensuring that all other properties of the vertex remain intact
            Vertex transformedVertex = v;
            transformedVertex.v = v.v * rotMat + transVec;
            transformedVertices.push_back(std::move(transformedVertex));
        }
        
        // determine which triangles should be culled
        std::vector<bool> cullTriangle;
        for (const auto& t : itl.triangles)
        {
            const Vertex& v1 = transformedVertices[t.indices[0]];
            const Vertex& v2 = transformedVertices[t.indices[1]];
            const Vertex& v3 = transformedVertices[t.indices[2]];
            
            // calculate the "normal" (which actually isn't normalized)
            Vec3 norm = ((v2.v - v1.v).cross(v3.v - v1.v));
            
            // cull if the "normal" is facing away from the camera - i.e. if the dot product
            // between the normal and a vector from the camera to any point on the triangle
            // (e.g. v1) is positive (or zero)
            cullTriangle.push_back((norm * v1.v) >= 0.0f);
        }
        
        // translate everything into screen space
        for (auto& v : transformedVertices)
            ScreenTransform::Transform(v);
        
        // draw textured triangles
        for (int i = 0; i < itl.triangles.size(); i++)
        {
            if (!cullTriangle[i])
            {
                const Triangle& t = itl.triangles[i];
                DrawTriangleTex(transformedVertices[t.indices[0]],
                                transformedVertices[t.indices[1]],
                                transformedVertices[t.indices[2]]);
            }
        }
        
        // draw lines
        //    for (const auto& l : ill.lines)
        //        g.DrawLine(ill.vertices[l.indices[0]], ill.vertices[l.indices[1]], Colors::White);
        
    }

private:
    // these follow triangle rasterization rules described at
    // https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules
    inline static int Rast(float n) { return static_cast<int>(ceil(n - 0.5f)); }
    void DrawLine(const Vec2& v1, const Vec2& v2, const Color& c)
    {
        DrawLine(v1.x, v1.y, v2.x, v2.y, c);
    }
    // taken from CS480 homework!
    // it is not perfect - it does not line up perfectly with the rasterization rules in
    // the triangle rasterization functions (which *are* perfect!)
    void DrawLine(float x1, float y1, float x2, float y2, const Color& c)
    {
        // if we've been passed a single point, don't bother drawing a line
        if (x1 == x2 && y1 == y2)
        {
            g.PutPixel(Rast(x1), Rast(y1), c);
        }
        else
        {
            int i;
            
            float x = x1, y = y1;
            
            int numPix; // the number of times to iterate
            int distX = Rast(x2) - Rast(x1); // the dimensions of the line
            int distY = Rast(y2) - Rast(y1);
            
            // should we scan horizontally or vertically?
            if (abs(distX) > abs(distY)) numPix = abs(distX);
            else numPix = abs(distY);
            
            // calculate the incremental x and y values (can be pos. or neg.)
            float deltaX = static_cast<float>(distX) / static_cast<float>(numPix);
            float deltaY = static_cast<float>(distY) / static_cast<float>(numPix);
            
            // draw the line
            for (i = 0; i < numPix; i++)
            {
                g.PutPixel(Rast(x), Rast(y), c);
                x += deltaX;
                y += deltaY;
            }
            g.PutPixel(Rast(x), Rast(y), c);
        }
    }
    void DrawTriangleTex(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        // rearrange vertices such that v1 is at the top and v3 is at the bottom
        const Vertex* pV1 = &v1;
        const Vertex* pV2 = &v2;
        const Vertex* pV3 = &v3;
        
        if (pV2->v.y < pV1->v.y)
            std::swap(pV1, pV2);
        
        if (pV3->v.y < pV2->v.y)
        {
            std::swap(pV2, pV3);
            if (pV2->v.y < pV1->v.y)
                std::swap(pV1, pV2);
        }
        
        if (pV1->v.y == pV2->v.y)
        {
            if (pV2->v.x < pV1->v.x)
                std::swap(pV1, pV2);
            DrawFlatTopTriangleTex(*pV1, *pV2, *pV3);
        }
        else if (pV2->v.y == pV3->v.y)
        {
            if (pV3->v.x < pV2->v.x)
                std::swap(pV2, pV3);
            DrawFlatBottomTriangleTex(*pV1, *pV2, *pV3);
        }
        else
        {
            float heightRatio = (pV2->v.y - pV1->v.y)/(pV3->v.y - pV1->v.y);
            Vertex vSplit = pV1->InterpTo(*pV3, heightRatio);
            
            if (vSplit.v.x < pV2->v.x)
            {
                DrawFlatBottomTriangleTex(*pV1, vSplit, *pV2);
                DrawFlatTopTriangleTex(vSplit, *pV2, *pV3);
            }
            else
            {
                DrawFlatBottomTriangleTex(*pV1, *pV2, vSplit);
                DrawFlatTopTriangleTex(*pV2, vSplit, *pV3);
            }
        }
    }
    //   v1 *-------* v2
    //       \     /
    //        \   /
    //         \ /
    //          * v3
    void DrawFlatTopTriangleTex(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        // for both the left side and right side of the triangle, calculate the (floating point) step
        // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
        // should be taken for each y scanline step
        float triangleHeight = v3.v.y - v1.v.y;
        Vertex stepPerYLeft = (v3 - v1) / triangleHeight;
        Vertex stepPerYRight = (v3 - v2) / triangleHeight;
        
        DrawFlatTriangleTex(v1, v2, v3, stepPerYLeft, stepPerYRight, v2);
    }
    //          * v1
    //         / \
    //        /   \
    //       /     \
    //   v2 *-------* v3
    void DrawFlatBottomTriangleTex(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        // for both the left side and right side of the triangle, calculate the (floating point) step
        // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
        // should be taken for each y scanline step
        float triangleHeight = v3.v.y - v1.v.y;
        Vertex stepPerYLeft = (v2 - v1) / triangleHeight;
        Vertex stepPerYRight = (v3 - v1) / triangleHeight;
        
        DrawFlatTriangleTex(v1, v2, v3, stepPerYLeft, stepPerYRight, v1);
    }
    void DrawFlatTriangleTex(const Vertex& v1, const Vertex& v2, const Vertex& v3,
                             const Vertex& stepPerYLeft, const Vertex& stepPerYRight, const Vertex& upperRightV)
    {
        // quantize the beginning (inclusive) and end (non-inclusive) y values for the top and bottom of
        // the triangle, following our rasterization rules
        int yStart = Rast(v1.v.y);
        int yEnd = Rast(v3.v.y);
        
        // *rough* initial values for starting/ending x, but...
        Vertex xStartVertex = v1; // (same for either flat top triangle or flat bottom triangle)
        Vertex xEndVertex = upperRightV;
        
        // this logic may not be intuitive - we need to "bump" the x start and end values a bit:
        // we actually should be calculating the x values based on the y value for the *vertical center*
        // of the first row of pixels (not the upper edge)
        float yOffsetForFirstRow = static_cast<float>(yStart) + 0.5f - v1.v.y;
        xStartVertex += stepPerYLeft * yOffsetForFirstRow;
        xEndVertex += stepPerYRight * yOffsetForFirstRow;
        
        Vertex currPixelVertex;
        Vertex stepPerX;
        
        for (int y = yStart; y < yEnd; y++)
        {
            // quantize the beginning (inclusive) and end (non-inclusive) x values for the left and right of
            // the triangle, following our rasterization rules
            int xStartI = Rast(xStartVertex.v.x);
            int xEndI = Rast(xEndVertex.v.x);
            
            // interpolate all aspects of a Vertex as we scan in the x direction
            // (we know that that the calculated "x step per x" will of course be 1.0f, and the "y step per x" will
            // of course be 0.0f - we don't really need to do this math, but that's ok - it simplifies the code)
            stepPerX = (xEndVertex - xStartVertex) / (xEndVertex.v.x - xStartVertex.v.x);
            
            // *rough* initial values for starting x, but...
            currPixelVertex = xStartVertex;
            
            // similar to above, we will "bump" the texture starting coordinates a bit based on
            // the horizontal center of the first column
            currPixelVertex += stepPerX * (static_cast<float>(xStartI) + 0.5f - xStartVertex.v.x);
            
            for (int x = xStartI; x < xEndI; x++)
            {
                // remember that the z member was "hacked" to actually represent 1/z during the object
                // space to screen space transformation (ScreenTransform class)!
                auto zInv = currPixelVertex.v.z;
                
                // recover attributes of the vertex which had previously been transformed by the screen-space
                // transformation
                Vertex currPixelVertexRecovered = currPixelVertex / zInv;
                g.PutPixel(x, y, effect.pixelShader(currPixelVertexRecovered));
                
                currPixelVertex += stepPerX;
            }
            
            xStartVertex += stepPerYLeft;
            xEndVertex += stepPerYRight;
        }
    }
    
    Graphics& g;
    Effect effect;
};

#endif /* Pipeline_hpp */
