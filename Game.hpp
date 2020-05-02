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
#include "Cube.hpp"
#include "Pipeline.hpp"
#include "TextureEffect.hpp"
#include "VertexColorEffect.hpp"

class Game
{
public:
    Game();
    ~Game() = default;
    void ComposeFrame();
    
private:
    Cube c;
    Graphics g;
    //Pipeline<VertexColorEffect> p;
    Pipeline<TextureEffect> p;
};

#endif /* Game_hpp */
