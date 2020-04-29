//
//  Vec3Tex.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/27/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Vec3Tex_hpp
#define Vec3Tex_hpp

#include "Vec3.hpp"
#include "Vec2.hpp"

class Vec3Tex
{
public:
    Vec3Tex InterpTo(const Vec3Tex& rhs, float percent) const
    {
        return { v.InterpTo(rhs.v, percent),
            textureCoords.InterpTo(rhs.textureCoords, percent) };
    }
    
    Vec3 v;
    Vec2 textureCoords;
};

#endif /* Vec3Tex_hpp */
