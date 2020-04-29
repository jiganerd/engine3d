//
//  main.cpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#include <iostream>
#include "Game.hpp"
#include "Mat3.hpp"
#include "Vec3.hpp"
#include "Input.hpp"

int main()
{
    try
    {
        Game g;

        while (!Input::HandleKeys())
        {
            g.ComposeFrame();
        }
    }
    catch (const Graphics::SDLException& e)
    {
        std::cerr << e.GetMsg() << std::endl;
    }
}