#pragma once

#include <cstdio>

template <typename T = float>
class Point3
{
public:
    constexpr Point3() = default;
    constexpr Point3(T e0, T e1, T e2) : m_e{e0, e1, e2} {}

    constexpr T x() const { return m_e[0]; }
    constexpr T y() const { return m_e[1]; }
    constexpr T z() const { return m_e[2]; }

private:
    T m_e[3] = {};
};

template <typename T>
void print(const Point3<T>& p)
{
    fprintf(stderr, "%f, %f, %f\n", p.x(), p.y(), p.z());
}
