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

Game::Game():
    pT(g),
    pVC(g),
    pFS(g),
    pG(g)
{
}

void Game::ComposeFrame()
{
    g.BeginFrame();
    
    int scene = 3;
    switch (scene)
    {
    case 0:
        {
        IndexedTriangleList<VertexColorEffect::Vertex> itlcvc = c.GetIndexedTriangleListVC();
        pVC.Draw(itlcvc);
        break;
        }
            
    case 1:
        {
        IndexedTriangleList<GouraudEffect::Vertex> itlsg = s.GetIndexedTriangleListG();
        pG.Draw(itlsg);
        break;
        }
        
    case 2:
        {
        IndexedTriangleList<FlatShadingEffect::Vertex> itlsfs = s.GetIndexedTriangleListFS();
        pFS.Draw(itlsfs);
        }
        break;
        
    case 3:
        {
        IndexedTriangleList<TextureEffect::Vertex> itlct = c.GetIndexedTriangleListTex();
        pT.Draw(itlct);
        }
        break;
    }
    
    g.EndFrame();
}