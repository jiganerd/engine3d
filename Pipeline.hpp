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
#include "Triangle.hpp"

template <typename Effect>
class Pipeline
{
    using Vertex = typename Effect::Vertex;
    using VertexShader = typename Effect::VertexShader;
    using VSOutVertex = typename Effect::VertexShader::OutVertex;
    using GSOutVertex = typename Effect::GeometryShader::OutVertex;
    using PixelShader = typename Effect::PixelShader;
    
public:
    Pipeline(Graphics& g):
        g(g)
    {}
    void Draw(const IndexedTriangleList<Vertex>& itl)
    {
        // run the vertex shader over all vertices
        std::vector<VSOutVertex> transformedVertices;
        for (const auto& v : itl.vertices)
            transformedVertices.push_back(effect.vertexShader(v));
        
        // determine which triangles should be culled
        for (const auto& t : itl.triangles)
        {
            const VSOutVertex& v1 = transformedVertices[t.indices[0]];
            const VSOutVertex& v2 = transformedVertices[t.indices[1]];
            const VSOutVertex& v3 = transformedVertices[t.indices[2]];
            
            // calculate the normal
            Vec3 norm = ((v2.v - v1.v).cross(v3.v - v1.v)).Norm();
            
            // cull if the normal is facing away from the camera - i.e. if the dot product
            // between the normal and a vector from the camera to any point on the triangle
            // (e.g. v1) is positive (or zero)
            if ((norm * v1.v) < 0.0f)
                ProcessTriangle(v1, v2, v3);
        }
    }
    
private:
    void ProcessTriangle(const VSOutVertex& v1, const VSOutVertex& v2, const VSOutVertex& v3)
    {
        Triangle<GSOutVertex> t = effect.geometryShader({v1, v2, v3});
        
        // translate everything into screen space
        ScreenTransform::Transform(t.v1);
        ScreenTransform::Transform(t.v2);
        ScreenTransform::Transform(t.v3);

        DrawTriangle(t.v1, t.v2, t.v3);
    }
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
    void DrawTriangle(const GSOutVertex& v1, const GSOutVertex& v2, const GSOutVertex& v3)
    {
        // rearrange vertices such that v1 is at the top and v3 is at the bottom
        const GSOutVertex* pV1 = &v1;
        const GSOutVertex* pV2 = &v2;
        const GSOutVertex* pV3 = &v3;
        
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
            DrawFlatTopTriangle(*pV1, *pV2, *pV3);
        }
        else if (pV2->v.y == pV3->v.y)
        {
            if (pV3->v.x < pV2->v.x)
                std::swap(pV2, pV3);
            DrawFlatBottomTriangle(*pV1, *pV2, *pV3);
        }
        else
        {
            float heightRatio = (pV2->v.y - pV1->v.y)/(pV3->v.y - pV1->v.y);
            GSOutVertex vSplit = pV1->InterpTo(*pV3, heightRatio);
            
            if (vSplit.v.x < pV2->v.x)
            {
                DrawFlatBottomTriangle(*pV1, vSplit, *pV2);
                DrawFlatTopTriangle(vSplit, *pV2, *pV3);
            }
            else
            {
                DrawFlatBottomTriangle(*pV1, *pV2, vSplit);
                DrawFlatTopTriangle(*pV2, vSplit, *pV3);
            }
        }
    }
    //   v1 *-------* v2
    //       \     /
    //        \   /
    //         \ /
    //          * v3
    void DrawFlatTopTriangle(const GSOutVertex& v1, const GSOutVertex& v2, const GSOutVertex& v3)
    {
        // for both the left side and right side of the triangle, calculate the (floating point) step
        // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
        // should be taken for each y scanline step
        float triangleHeight = v3.v.y - v1.v.y;
        GSOutVertex stepPerYLeft = (v3 - v1) / triangleHeight;
        GSOutVertex stepPerYRight = (v3 - v2) / triangleHeight;
        
        DrawFlatTriangle(v1, v2, v3, stepPerYLeft, stepPerYRight, v2);
    }
    //          * v1
    //         / \
    //        /   \
    //       /     \
    //   v2 *-------* v3
    void DrawFlatBottomTriangle(const GSOutVertex& v1, const GSOutVertex& v2, const GSOutVertex& v3)
    {
        // for both the left side and right side of the triangle, calculate the (floating point) step
        // in the x direction and the (2-dimensional floating point) step in the texture map lookup that
        // should be taken for each y scanline step
        float triangleHeight = v3.v.y - v1.v.y;
        GSOutVertex stepPerYLeft = (v2 - v1) / triangleHeight;
        GSOutVertex stepPerYRight = (v3 - v1) / triangleHeight;
        
        DrawFlatTriangle(v1, v2, v3, stepPerYLeft, stepPerYRight, v1);
    }
    void DrawFlatTriangle(const GSOutVertex& v1, const GSOutVertex& v2, const GSOutVertex& v3,
                          const GSOutVertex& stepPerYLeft, const GSOutVertex& stepPerYRight, const GSOutVertex& upperRightV)
    {
        // quantize the beginning (inclusive) and end (non-inclusive) y values for the top and bottom of
        // the triangle, following our rasterization rules
        int yStart = Rast(v1.v.y);
        int yEnd = Rast(v3.v.y);
        
        // *rough* initial values for starting/ending x, but...
        GSOutVertex xStartVertex = v1; // (same for either flat top triangle or flat bottom triangle)
        GSOutVertex xEndVertex = upperRightV;
        
        // this logic may not be intuitive - we need to "bump" the x start and end values a bit:
        // we actually should be calculating the x values based on the y value for the *vertical center*
        // of the first row of pixels (not the upper edge)
        float yOffsetForFirstRow = static_cast<float>(yStart) + 0.5f - v1.v.y;
        xStartVertex += stepPerYLeft * yOffsetForFirstRow;
        xEndVertex += stepPerYRight * yOffsetForFirstRow;
        
        GSOutVertex currPixelVertex;
        GSOutVertex stepPerX;
        
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
                GSOutVertex currPixelVertexRecovered = currPixelVertex / zInv;
                g.PutPixel(x, y, effect.pixelShader(currPixelVertexRecovered));
                
                currPixelVertex += stepPerX;
            }
            
            xStartVertex += stepPerYLeft;
            xEndVertex += stepPerYRight;
        }
    }
    
    Graphics& g;
    
public:
    Effect effect;
};

#endif /* Pipeline_hpp */
