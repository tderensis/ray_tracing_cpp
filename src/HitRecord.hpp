#pragma once

#include "Point.hpp"
#include "Vec.hpp"

template <typename T>
class Material;

template <typename T>
struct HitRecord
{
    T t{};
    Point3<T> p{0, 0, 0};
    Vec3<T> normal{0, 0, 0};
    Material<T>* material{nullptr};
};
