//
//  Game.cpp
//  engine3d
//
//  Created by Brian Dolan on 4/25/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include "Game.hpp"
#include "IndexedTriangleList.hpp"
#include "TextureEffect.hpp"
#include "Utils.hpp"

Game::Game():
    pT(g),
    pVC(g),
    pFS(g),
    pG(g)
{
    // push objects away from the camera as clipping is not currently handled
    Vec3 transVec(0.0f, 0.0f, 2.0f);
    pT.effect.vertexShader.BindTranslation(transVec);
    pVC.effect.vertexShader.BindTranslation(transVec);
    pFS.effect.vertexShader.BindTranslation(transVec);
    pG.effect.vertexShader.BindTranslation(transVec);
}

bool Game::ProcessFrame()
{
    bool quit = i.ProcessKeys();
    
    if (!quit)
    {
        HandleInput();
        
        g.BeginFrame();
        ComposeFrame();
        g.EndFrame();

        frm.Mark();
    }
    
    return quit;
}

void Game::ComposeFrame()
{
    switch (sceneNum)
    {
    // textured, flat-shaded cube
    case 0:
    {
        IndexedTriangleList<TextureEffect::Vertex> itlct = c.GetIndexedTriangleListTex();
        pT.Draw(itlct);
        break;
    }
            
    // vertex-colored cube
    case 1:
    {
        IndexedTriangleList<VertexColorEffect::Vertex> itlcvc = c.GetIndexedTriangleListVC();
        pVC.Draw(itlcvc);
        break;
    }
            
    // flat-shaded sphere
    case 2:
    {
        IndexedTriangleList<FlatShadingEffect::Vertex> itlsfs = s.GetIndexedTriangleListFS();
        pFS.Draw(itlsfs);
        break;
    }

    // gouraud-shaded sphere
    case 3:
    {
        IndexedTriangleList<GouraudEffect::Vertex> itlsg = s.GetIndexedTriangleListG();
        pG.Draw(itlsg);
        break;
    }
            
    default:
        break;
    }
}

void Game::HandleInput()
{
    // handle scene switching
    
    if (i.GetTabFirstPressed())
        if (++sceneNum > 3)
            sceneNum = 0;
    
    // handle rotation, with speed based on frame rate
    
    float rotSpeed = frm.GetFrameTimeSecs() * Utils::Pi;
        
    if (i.GetRotateLeft() || i.GetRotateRight())
    {
        rotYAngle += rotSpeed * (i.GetRotateRight() ? -1.0f : 1.0f);
        Utils::NormalizeAngle(rotYAngle);
    }
    
    if (i.GetMoveForward() || i.GetMoveBackward())
    {
        rotXAngle += rotSpeed * (i.GetMoveBackward() ? -1.0f : 1.0f);
        Utils::NormalizeAngle(rotXAngle);
    }
    
    Mat3 rotMat = Mat3::RotY(rotYAngle) * Mat3::RotX(rotXAngle);
    pT.effect.vertexShader.BindRotation(rotMat);
    pVC.effect.vertexShader.BindRotation(rotMat);
    pFS.effect.vertexShader.BindRotation(rotMat);
    pG.effect.vertexShader.BindRotation(rotMat);
}