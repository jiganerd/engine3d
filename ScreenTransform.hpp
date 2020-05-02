//
//  ScreenTransform.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef ScreenTransform_hpp
#define ScreenTransform_hpp

#include "Graphics.hpp"
#include "Vec3.hpp"

// this class transforms objects in a coordinate system where the screen width and height
// range from -1 to +1 to the actual screen dimensions in terms of pixels
class ScreenTransform
{
public:
    ScreenTransform() = delete;
    
    // this function takes a Vertex in object space (x, y, z, other attributes) and translates it
    // into screen space, with an output as follows:
    // - x, y coordinates as screen coordinates
    // - z is "hacked" to actually hold 1/z!
    // - all other attributes hold (their original value)/z - values divided by z can be linearly
    //   interpolated while moving across screen space, e.g. for perspective-correct texture mapping
    template <typename Vertex>
    static void Transform(Vertex& v)
    {
        auto zInv = 1.0f/v.v.z;
        
        // divide all attributes by z
        v *= zInv;
        
        // translate x and y from object space to screen space
        v.v.x = (v.v.x + 1.0f) * HalfScreenWidth;
        v.v.y = (-v.v.y + 1.0f) * HalfScreenHeight;
        
        // "hack" the z member to actually hold 1/z - this will be used later when rendering
        // to recover attributes, such as texture u/v coordinates
        v.v.z = zInv;
    }
    ~ScreenTransform() = delete;
    
public:
    static constexpr float HalfScreenHeight = static_cast<float>(Graphics::ScreenHeight) / 2.0f;
    static constexpr float HalfScreenWidth = static_cast<float>(Graphics::ScreenWidth) / 2.0f;
};

#endif /* ScreenTransform_hpp */
