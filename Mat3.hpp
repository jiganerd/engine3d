//
//  Mat3.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Mat3_hpp
#define Mat3_hpp

#include "Vec3.hpp"

template<typename T>
class _Mat3
{
public:
    _Mat3() = default;
    _Mat3<T> operator+(const _Mat3<T>& rhs)
    {
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                data[i][j] += rhs[i][j];
            }
        }
    }
    _Mat3<T>& operator+=(const _Vec3<T>& rhs)
    {
        data[0][0] += rhs.x;
        data[1][1] += rhs.y;
        data[2][2] += rhs.z;
        return *this;
    }
    _Mat3<T> operator*(const _Mat3<T>& rhs)
    {
        _Mat3<T> ret;
        
        // loop through rows (r) and columns (c) for the *output* matrix
        for (size_t r = 0; r < 3; r++)
        {
            for (size_t c = 0; c < 3; c++)
            {
                ret.data[r][c] = z;
                for (size_t i = 0; i < 3; i++)
                    ret.data[r][c] += (data[i][c] * rhs.data[r][i]);
            }
        }
        
        return ret;
    }
    static _Mat3<T> Identity()
    {
        return
        {
            o, z, z,
            z, o, z,
            z, z, o
        };
    }
    static _Mat3<T> RotZ(T angle)
    {
        T s = static_cast<T>(sin(angle));
        T c = static_cast<T>(cos(angle));
        return
        {
            c, s, z,
            -s, c, z,
            z, z, o
        };
    }
    static _Mat3<T> RotY(T angle)
    {
        T s = static_cast<T>(sin(angle));
        T c = static_cast<T>(cos(angle));
        return
        {
            c, 0, -s,
            z, o, z,
            s, z, c
        };
    }
    static _Mat3<T> RotX(T angle)
    {
        T s = static_cast<T>(sin(angle));
        T c = static_cast<T>(cos(angle));
        return
        {
            o, z, z,
            z, c, s,
            z, -s, c
        };
    }

    ~_Mat3() = default;
    
private:
    static constexpr T z = static_cast<T>(0);
    static constexpr T o = static_cast<T>(1);
public:
    T data[3][3];
};

template<typename T>
_Vec3<T> operator*(const _Vec3<T>& lhs, const _Mat3<T>& rhs)
{
    _Vec3<T> res;
    res.x = lhs.x * rhs.data[0][0] + lhs.y * rhs.data[1][0] + lhs.z * rhs.data[2][0];
    res.y = lhs.x * rhs.data[0][1] + lhs.y * rhs.data[1][1] + lhs.z * rhs.data[2][1];
    res.z = lhs.x * rhs.data[0][2] + lhs.y * rhs.data[1][2] + lhs.z * rhs.data[2][2];
    return res;
}

template<typename T>
_Vec3<T>& operator*=(_Vec3<T>& lhs, const _Mat3<T>& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

typedef _Mat3<float> Mat3;

#endif /* Mat3_hpp */
