#pragma once

#include "Vec.hpp"
#include "Point.hpp"
#include "Color.hpp"

template <typename T>
inline Point3<T> operator+(const Point3<T>& p, const Vec3<T>& v)
{
    return {
        p.x() + v.x(),
        p.y() + v.y(),
        p.z() + v.z()
    };
}

template <typename T>
inline Point3<T> operator+(const Vec3<T>& v, const Point3<T>& p)
{
    return {
        p.x() + v.x(),
        p.y() + v.y(),
        p.z() + v.z()
    };
}

template <typename T>
inline Point3<T> operator-(const Point3<T>& p, const Vec3<T>& v)
{
    return {
        p.x() - v.x(),
        p.y() - v.y(),
        p.z() - v.z()
    };
}

template <typename T>
inline Point3<T> operator-(const Vec3<T>& v, const Point3<T>& p)
{
    return {
        v.x() - p.x(),
        v.y() - p.y(),
        v.z() - p.z()
    };
}

template <typename T>
inline T dot(const Point3<T>& p, const Vec3<T>& v)
{
    return p.x() * v.x() + p.y() * v.y() + p.z() * v.z();
}

template <typename T>
inline T dot(const Vec3<T>& v, const Point3<T>& p)
{
    return p.x() * v.x() + p.y() * v.y() + p.z() * v.z();
}

template <typename T>
inline Vec3<T> make_vec(const Point3<T>& p1, const Point3<T>& p2)
{
    return {
        p1.x() - p2.x(),
        p1.y() - p2.y(),
        p1.z() - p2.z()
    };
}

template <typename T>
inline Color<T> operator*(const Vec3<T>& v, const Color<T>& c)
{
    return {
        v.x() * c.r(),
        v.y() * c.g(),
        v.z() * c.b()
    };
}

template <typename T>
inline Color<T> operator*(const Color<T>& c, const Vec3<T>& v)
{
    return {
        v.x() * c.r(),
        v.y() * c.g(),
        v.z() * c.b()
    };
}
