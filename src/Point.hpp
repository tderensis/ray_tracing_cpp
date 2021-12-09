#pragma once

template <typename T = float>
class Point3
{
public:
    Point3() = default;
    Point3(T e0, T e1, T e2) : m_e{e0, e1, e2} {}

    inline T x() const { return m_e[0]; }
    inline T y() const { return m_e[1]; }
    inline T z() const { return m_e[2]; }

private:
    T m_e[3] = {};
};

template <typename T>
void print(const Point3<T>& p)
{
    fprintf(stderr, "%f, %f, %f\n", p.x(), p.y(), p.z());
}
