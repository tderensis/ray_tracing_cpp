#pragma once

template <typename POINT, typename VEC>
class Ray3
{
public:
    Ray3() = default;
    Ray3(const POINT& a, const VEC& b) : m_a{a}, m_b{b} {}

    POINT origin()  const { return m_a; };
    VEC direction() const { return m_b; };

    template <typename T>
    POINT point_at_parameter(T t) const { return m_a + t*m_b; };

private:
    POINT m_a{};
    VEC m_b{};
};
