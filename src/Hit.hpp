#pragma once

#include "Point.hpp"
#include "Vec.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "HitRecord.hpp"

template <typename T>
bool hit(
    const Sphere3<T>& sphere,
    const Ray3<Point3<T>, Vec3<T>>& ray,
    T t_min, T t_max, HitRecord<T>& record)
{
    auto oc = make_vec(ray.origin(), sphere.center());
    auto a = dot(ray.direction(), ray.direction());
    auto b = 2 * dot(oc, ray.direction());
    auto c = dot(oc, oc) - sphere.radius() * sphere.radius();
    auto discriminant = b * b - 4 * a * c;
    if (discriminant > 0)
    {
        auto temp = -(b + sqrt(discriminant)) / (2 * a);
        if (temp < t_max && temp > t_min)
        {
            record.t = temp;
            record.p = ray.point_at_parameter(record.t);
            record.normal = make_vec(record.p, sphere.center()) * (1 / sphere.radius());
            record.material = sphere.material();
            return true;
        }
        temp = -(b - sqrt(discriminant)) / (2 * a);
        if (temp < t_max && temp > t_min)
        {
            record.t = temp;
            record.p = ray.point_at_parameter(record.t);
            record.normal = make_vec(record.p, sphere.center()) * (1 / sphere.radius());
            record.material = sphere.material();
            return true;
        }
    }
    return false;
}
