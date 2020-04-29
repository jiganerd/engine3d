//
//  IndexedTriangleList.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/26/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef IndexedTriangleList_hpp
#define IndexedTriangleList_hpp

#include <vector>
#include "Vec3.hpp"

struct Triangle
{
    size_t indices[3];
};

template<typename T>
struct IndexedTriangleList
{
    std::vector<T> vertices;
    std::vector<Triangle> triangles;
    std::vector<bool> cullTriangle;
};

#endif /* IndexedTriangleList_hpp */
