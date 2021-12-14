#pragma once

#include "Point.hpp"
#include "Material.hpp"
#include "Hit.hpp"

template <typename T>
class Sphere3 : public Hittable<T>
{
public:
    Sphere3() = default;
    Sphere3(Point3<T> center, T radius, const Material<T>* material)
        : m_center{center}, m_radius{radius}, m_material{material}
    {}

    Point3<T> center() const { return m_center; }
    T radius() const { return m_radius; }
    const Material<T>* material() const { return m_material; }

    bool hit(
         const Ray3<Point3<T>, Vec3<T>>& ray,
         T t_min,
         T t_max,
         HitRecord<T>& record) const override
    {
        auto oc = make_vec(ray.origin(), m_center);
        auto a = ray.direction().squared_length();
        auto half_b = dot(oc, ray.direction());
        auto c = oc.squared_length() - m_radius * m_radius;
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
             root = (-half_b + sqrt_discriminant) / a;
             if (root < t_min || root > t_max)
             {
                 return false;
             }
        }

        record.t = root;
        record.p = ray.point_at_parameter(record.t);

        auto outward_normal = make_vec(record.p, m_center) / m_radius;

        record.front_face = dot(ray.direction(), outward_normal) < 0;
        record.normal = record.front_face ?
            outward_normal : -outward_normal;
        record.material = m_material;

        return true;
    }

private:
    Point3<T> m_center{};
    T m_radius{};
    const Material<T>* m_material{nullptr};
};
