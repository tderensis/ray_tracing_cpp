#pragma once

#include "Point.hpp"
#include "Vec.hpp"
#include "Ray.hpp"

template <typename T>
Vec3<T> random_in_unit_disk(std::mt19937& eng)
{
    std::uniform_real_distribution<T> distr(-1, 1);
    Vec3<T> p{0, 0, 0};
    do {
        p = Vec3<T>{distr(eng), distr(eng), 0};
    } while (p.squared_length() >= 1);
    return p;
}

template <typename T>
class Camera
{
public:
    Camera(std::mt19937& eng,
           Point3<T> lookfrom,
           Point3<T> lookat,
           Vec3<T> vup,
           T vfov,
           T aspect,
           T apeture,
           T focus_dist) : m_random_engine{eng}
    {
        T theta = vfov * M_PI / 180;
        T half_height = tan(theta/2);
        T half_width = aspect * half_height;

        m_w = unit_vector(make_vec(lookfrom, lookat));
        m_u = unit_vector(cross(vup, m_w));
        m_v = cross(m_w, m_u);

        m_origin = lookfrom;
        m_lower_left_corner = m_origin -
            focus_dist * half_width * m_u -
            focus_dist * half_height * m_v -
            focus_dist * m_w;
        m_horizontal = 2 * focus_dist * half_width * m_u;
        m_vertical   = 2 * focus_dist * half_height * m_v;
        m_lens_radius = apeture / 2;
    }

    Ray3<Point3<T>, Vec3<T>> get_ray(T u, T v) const
    {
        auto rd = m_lens_radius * random_in_unit_disk<T>(m_random_engine);
        auto offset = m_u * rd.x() + m_v * rd.y();
        return {
            m_origin + offset,
            make_vec(m_lower_left_corner +
                     u * m_horizontal +
                     v * m_vertical, m_origin + offset)
        };
    }
private:
    std::mt19937& m_random_engine;
    Point3<T> m_lower_left_corner;
    Vec3<T> m_horizontal;
    Vec3<T> m_vertical;
    Point3<T> m_origin;
    Vec3<T> m_u, m_v, m_w;
    T m_lens_radius;
};
