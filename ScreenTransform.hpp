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
    static void Transform(Vec3& v)
    {
        float zInv = 1.0f/v.z;
        v.x = (v.x * zInv + 1.0f) * HalfScreenWidth;
        v.y = (-v.y * zInv + 1.0f) * HalfScreenHeight;
    }
    ~ScreenTransform() = delete;
    
public:
    static constexpr float HalfScreenHeight = static_cast<float>(Graphics::ScreenHeight) / 2.0f;
    static constexpr float HalfScreenWidth = static_cast<float>(Graphics::ScreenWidth) / 2.0f;
};

#endif /* ScreenTransform_hpp */
