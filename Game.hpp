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
#include "ScreenTransform.hpp"
#include "Cube.hpp"

class Game
{
public:
    Game();
    ~Game() = default;
    void ComposeFrame();
    
private:
    Cube c;
    Graphics g;
    Surface s;
};

#endif /* Game_hpp */
