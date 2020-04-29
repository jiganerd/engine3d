//
//  Mat2.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Mat2_hpp
#define Mat2_hpp

template<typename T>
class Mat2
{
public:
    Mat2() = default;
    ~Mat2() = default;

private:
    T data[2][2];
};

#endif /* Mat2_hpp */
