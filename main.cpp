//
//  main.cpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <iostream>
#include "Game.hpp"

int main()
{
    try
    {
        Game g;
        bool quit = false;

        while (!quit)
        {
            quit = g.ProcessFrame();
        }
    }
    catch (const Graphics::Exception& e)
    {
        std::cerr << e.GetMsg() << std::endl;
    }
}