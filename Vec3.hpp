//
//  Vec3.hpp
//  engine3d
//
//  Created by Brian Dolan on 4/24/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef Vec3_hpp
#define Vec3_hpp

#include "Vec2.hpp"

template<typename T>
class _Vec3 : public _Vec2<T>
{
public:
    _Vec3() = default;
    _Vec3(T x, T y, T z):
        _Vec2<T>(x, y),
        z(z)
    {}
    _Vec3& operator=(const _Vec3& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }
    _Vec3 operator+(const _Vec3& rhs) const
    {
        return _Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    _Vec3& operator+=(const _Vec3& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
    _Vec3 operator-(const _Vec3& rhs) const
    {
        return _Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    _Vec3& operator-=(const _Vec3& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    _Vec3& operator-() const
    {
        return (*this * static_cast<T>(-1));
    }
    _Vec3 operator*(const T& rhs) const
    {
        return _Vec3(x * rhs, y * rhs, z * rhs);
    }
    _Vec3& operator*=(const T& rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    _Vec3 operator/(const T& rhs) const
    {
        return _Vec3(x / rhs, y / rhs, z / rhs);
    }
    _Vec3& operator/=(const _Vec3& rhs)
    {
        *this = *this / rhs;
        return *this;
    }
    T operator*(const _Vec3& rhs) const // dot product
    {
        return (x * rhs.x + y * rhs.y + z * rhs.z);
    }
    _Vec3 cross(const _Vec3& rhs) const // cross product
    {
        return _Vec3(y * rhs.z - z * rhs.y,
             z * rhs.x - x * rhs.z,
             x * rhs.y - y * rhs.x);
    }
    T MagSq() const
    {
        return (x*x + y*y + z*z);
    }
    T Mag() const
    {
        return sqrt(MagSq());
    }
    _Vec3 Norm() const
    {
        T mag = Mag();
        return _Vec3(x/mag, y/mag, z/mag);
    }
    _Vec3 InterpTo(const _Vec3& rhs, T percent) const
    {
        return (*this + (rhs - *this) * percent);
    }
    ~_Vec3() = default;
    
public:
    // these usings are due to a strange issue described at:
    // https://stackoverflow.com/questions/6592512/templates-parent-class-member-variables-not-visible-in-inherited-class
    // https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
    using _Vec2<T>::x;
    using _Vec2<T>::y;
    T z;
};

typedef _Vec3<float> Vec3;

#endif /* Vec3_hpp */
