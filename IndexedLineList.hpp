//
//  IndexedLineList.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/25/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef IndexedLineList_hpp
#define IndexedLineList_hpp

#include <vector>
#include "Vec3.hpp"

struct IndexedLine
{
    size_t indices[2];
};

struct IndexedLineList
{
    std::vector<Vec3> vertices;
    std::vector<IndexedLine> lines;
};

#endif /* IndexedLineList_hpp */
