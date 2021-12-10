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
    auto a = ray.direction().squared_length();
    auto half_b = dot(oc, ray.direction());
    auto c = oc.squared_length() - sphere.radius() * sphere.radius();
    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0)
    {
            return false;
    }

    auto sqrt_discriminant = sqrt(discriminant);

    // Find the nearest root that is in the range
    auto root = (-half_b - sqrt_discriminant) / a;

    if (root < t_min || root > t_max)
    {
        root = -(half_b - sqrt_discriminant) / a;
        if (root < t_min || root > t_max)
        {
            return false;
        }
    }

    record.t = root;
    record.p = ray.point_at_parameter(record.t);
    record.normal = make_vec(record.p, sphere.center()) / sphere.radius();
    record.material = sphere.material();

    return true;
}
