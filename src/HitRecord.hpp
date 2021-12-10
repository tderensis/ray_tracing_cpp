#pragma once

#include "Point.hpp"
#include "Vec.hpp"

template <typename T>
class Material;

template <typename T>
struct HitRecord
{
    T t;
    Point3<T> p;
    Vec3<T> normal;
    const Material<T>* material;
    bool front_face;
};
