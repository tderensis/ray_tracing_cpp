#pragma once
#include <cmath>
#include <cstdio>

template <typename T = float>
class Vec3
{
public:
    Vec3() = default;
    Vec3(T e0, T e1, T e2) : m_e{e0, e1, e2} {}

    inline T x() const { return m_e[0]; }
    inline T y() const { return m_e[1]; }
    inline T z() const { return m_e[2]; }

    inline Vec3<T> operator-() const { return {-m_e[0], -m_e[1], -m_e[2]}; }
    inline T length() const
    {
        return sqrt(squared_length());
    }

    inline T squared_length() const
    {
        return m_e[0] * m_e[0] +
               m_e[1] * m_e[1] +
               m_e[2] * m_e[2];
    }

    inline void make_unit_vector()
    {
        auto k = 1 / length();
        m_e[0] *= k;
        m_e[1] *= k;
        m_e[2] *= k;
    }
private:
    T m_e[3] = {};
};

template <typename T>
inline Vec3<T> unit_vector(Vec3<T> v)
{
    v.make_unit_vector();
    return v;
}

template <typename T>
inline T dot(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
}

template <typename T>
inline Vec3<T> cross(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return {
        v1.y() * v2.z() - v1.z() * v2.y(),
        v1.z() * v2.x() - v1.x() * v2.z(),
        v1.x() * v2.y() - v1.y() * v2.x()
    };
}


template <typename T>
Vec3<T> reflect(const Vec3<T>& v, const Vec3<T>& n)
{
    return v - 2 * dot(v, n) * n;
}

template <typename T>
inline Vec3<T> operator+(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>{
        v1.x() + v2.x(),
        v1.y() + v2.y(),
        v1.z() + v2.z()
    };
}

template <typename T>
inline Vec3<T> operator-(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>{
        v1.x() - v2.x(),
        v1.y() - v2.y(),
        v1.z() - v2.z()
    };
}

template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>{
        v1.x() * v2.x(),
        v1.y() * v2.y(),
        v1.z() * v2.z()
    };
}

template <typename T>
inline Vec3<T> operator*(T t, const Vec3<T>& v)
{
    return Vec3<T>{
        t * v.x(),
        t * v.y(),
        t * v.z()
    };
}

template <typename T>
inline Vec3<T> operator*(const Vec3<T>& v, T t)
{
    return Vec3<T>{
        t * v.x(),
        t * v.y(),
        t * v.z()
    };
}

template <typename T>
inline Vec3<T> operator/(const Vec3<T>& v, T t)
{
    return Vec3<T>{
        v.x() / t,
        v.y() / t,
        v.z() / t
    };
}

template <typename T>
void print(const Vec3<T>& v)
{
    fprintf(stderr, "%f, %f, %f\n", v.x(), v.y(), v.z());
}