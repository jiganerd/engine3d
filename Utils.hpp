//
//  Utils.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/28/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <algorithm>

class Utils
{
public:
    Utils() = delete;
    ~Utils() = delete;
    
    template <typename T>
    static void Clamp(T& n, const T& min, const T& max)
    {
        n = std::max(min, std::min(n, max));
    };
};

#endif /* Utils_hpp */
