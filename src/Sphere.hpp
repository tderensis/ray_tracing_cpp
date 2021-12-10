#pragma once

#include "Point.hpp"
#include "Material.hpp"

template <typename T>
class Sphere3
{
public:
    Sphere3() = default;
    Sphere3(Point3<T> center, T radius, const Material<T>* material)
        : m_center{center}, m_radius{radius}, m_material{material}
    {}

    Point3<T> center() const { return m_center; }
    T radius() const { return m_radius; }
    const Material<T>* material() const { return m_material; }
private:
    Point3<T> m_center{};
    T m_radius{};
    const Material<T>* m_material{nullptr};
};
