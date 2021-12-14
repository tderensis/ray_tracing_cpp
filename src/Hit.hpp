#pragma once

#include "Point.hpp"
#include "Vec.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"

template <typename T>
class Hittable
{
    virtual bool hit(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        T t_min, T t_max,
        HitRecord<T>& hit_record) const = 0;
};
