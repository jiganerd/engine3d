//
//  Game.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/25/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp

#include "Graphics.hpp"
#include "Input.hpp"
#include "Cube.hpp"
#include "Sphere.hpp"
#include "Pipeline.hpp"
#include "TextureEffect.hpp"
#include "VertexColorEffect.hpp"
#include "FlatShadingEffect.hpp"
#include "FrameRateMgr.hpp"

class Game
{
public:
    Game();
    ~Game() = default;
    bool ProcessFrame();
    
private:
    void ComposeFrame();
    void HandleInput();
    
    Cube c;
    Sphere s;

    Graphics g;
    
    Pipeline<TextureEffect> pT;
    Pipeline<VertexColorEffect> pVC;
    Pipeline<FlatShadingEffect> pFS;
    Pipeline<GouraudEffect> pG;
    
    Input i;
    FrameRateMgr frm;

    int sceneNum = 0;    
    float rotYAngle = 0.0f;
    float rotXAngle = 0.0f;
};

#endif /* Game_hpp */
