#pragma once
#include <cmath>
#include <cstdio>

template <typename T = float>
class Color
{
public:
    Color() = default;
    Color(T r, T g, T b) : m_r{r}, m_g{g}, m_b{b} {}

    T r() const { return m_r; }
    T g() const { return m_g; }
    T b() const { return m_b; }

private:
    T m_r{};
    T m_g{};
    T m_b{};
};

template <typename T>
inline Color<T> operator+(const Color<T>& v1, const Color<T>& v2)
{
    return { v1.r() + v2.r(), v1.g() + v2.g(), v1.b() + v2.b() };
}

template <typename T>
inline Color<T> operator-(const Color<T>& c1, const Color<T>& c2)
{
    return { c1.r() - c2.r(), c1.g() - c2.g(), c1.b() - c2.b() };
}

template <typename T>
inline Color<T> operator*(T t, const Color<T>& c)
{
    return { t * c.r(), t * c.g(), t * c.b() };
}

template <typename T>
inline Color<T> operator*(const Color<T>& c, T t)
{
    return { t * c.r(), t * c.g(), t * c.b() };
}

template <typename T>
inline Color<T> operator/(const Color<T>& c, T t)
{
    return { c.r() / t, c.g() / t, c.b() / t };
}

template <typename T>
inline Color<T> gamma_correct(const Color<T>& c)
{
    return { sqrt(c.r()), sqrt(c.g()), sqrt(c.b()) };
}

template <typename T>
void print(const Color<T>& c)
{
    fprintf(stderr, "r:%f, g:%f, b:%f\n", c.r(), c.g(), c.b());
}
