//
//  Game.cpp
//  engine3d
//
//  Created by Brian Dolan on 4/25/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include "Game.hpp"
#include "Color.hpp"
#include "Mat3.hpp"

Game::Game():
    s(g.LoadTexture("brick.bmp"))
{
}

void Game::ComposeFrame()
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
    
    g.BeginFrame();
    
    static float angle = 0.1;
    angle += 0.002;
    
    IndexedLineList ill = c.GetIndexedLineList();
    IndexedTriangleList<Vec3> itl = c.GetIndexedTriangleList();
    IndexedTriangleList<Vec3Tex> itlt = c.GetIndexedTriangleListTex();

    Mat3 rotMat = Mat3::RotZ(angle) * Mat3::RotX(angle*2) * Mat3::RotY(angle/2);
    Vec3 transVec(0.0f, 0.0f, 2.0f);
    
    // translate everything in world space
    for (auto& v : ill.vertices)
    {
        v *= rotMat;
        v += transVec;
    }
    for (auto& v : itl.vertices)
    {
        v *= rotMat;
        v += transVec;
    }
    for (auto& v : itlt.vertices)
    {
        v.v *= rotMat;
        v.v += transVec;
    }
    
    // determine which triangles should be culled
    for (const auto& t : itl.triangles)
    {
        const auto& vs = itl.vertices;
        const Vec3& v1 = vs[t.indices[0]];
        const Vec3& v2 = vs[t.indices[1]];
        const Vec3& v3 = vs[t.indices[2]];
        
        // calculate the "normal" (which actually isn't normalized)
        Vec3 norm = ((v2 - v1).cross(v3 - v1));
        
        itl.cullTriangle.push_back((norm * v1) >= 0.0f);
    }
    for (const auto& t : itlt.triangles)
    {
        const auto& vs = itl.vertices;
        const Vec3& v1 = vs[t.indices[0]];
        const Vec3& v2 = vs[t.indices[1]];
        const Vec3& v3 = vs[t.indices[2]];
        
        // calculate the "normal" (which actually isn't normalized)
        Vec3 norm = ((v2 - v1).cross(v3 - v1));
        
        itlt.cullTriangle.push_back((norm * v1) >= 0.0f);
    }
    
    // translate everything into screen space
    for (auto& v : ill.vertices)
        ScreenTransform::Transform(v);
    for (auto& v : itl.vertices)
        ScreenTransform::Transform(v);
    for (auto& v : itlt.vertices)
        ScreenTransform::Transform(v.v);
    
//    // draw untextured triangles
//    for (int i = 0; i < itl.triangles.size(); i++)
//    {
//        if (!itl.cullTriangle[i])
//        {
//            const Triangle& t = itl.triangles[i];
//            const auto& vs = itl.vertices;
//            g.DrawTriangle(vs[t.indices[0]], vs[t.indices[1]], vs[t.indices[2]], colors[i]);
//        }
//    }
    
    // draw textured triangles
    for (int i = 0; i < itlt.triangles.size(); i++)
    {
        if (!itlt.cullTriangle[i])
        {
            const Triangle& t = itlt.triangles[i];
            const auto& vs = itlt.vertices;
            g.DrawTriangleTex(vs[t.indices[0]], vs[t.indices[1]], vs[t.indices[2]], s);
        }
    }
    
    // draw lines
//    for (const auto& l : ill.lines)
//        g.DrawLine(ill.vertices[l.indices[0]], ill.vertices[l.indices[1]], Colors::White);
    
    g.EndFrame();
}