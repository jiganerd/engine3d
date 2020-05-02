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
    p(g)
{
}

void Game::ComposeFrame()
{
    g.BeginFrame();
    IndexedTriangleList<TextureEffect::Vertex> itl = c.GetIndexedTriangleListTex();
    //IndexedTriangleList<VertexColorEffect::Vertex> itl = c.GetIndexedTriangleListVC();
    p.Draw(itl);
    g.EndFrame();
}