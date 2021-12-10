#pragma once

#include "HitRecord.hpp"
#include "Ray.hpp"

#include "Random.hpp"

template <typename T>
class Material
{
public:
    virtual bool scatter(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        const HitRecord<T>& hit_record,
        Vec3<T>& attenuation,
        Ray3<Point3<T>, Vec3<T>>& scattered,
        Rng& rng) const = 0;
};

template <typename T>
Vec3<T> random_in_unit_sphere(Rng& rng)
{
    Vec3<T> p{0, 0, 0};
    do {
        p = Vec3<T>{
            rng.random<T>(-1, 1),
            rng.random<T>(-1, 1),
            rng.random<T>(-1, 1)
        };
    } while (p.squared_length() >= 1);
    return p;
}

template <typename T>
class Lambertian : public Material<T>
{
public:
    Lambertian(const Vec3<T>& a = {}) : m_albedo{a} {}

    virtual bool scatter(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        const HitRecord<T>& hit_record,
        Vec3<T>& attenuation,
        Ray3<Point3<T>, Vec3<T>>& scattered,
        Rng& rng) const override
    {
        auto target = hit_record.p + hit_record.normal +
            random_in_unit_sphere<T>(rng);
        scattered = {hit_record.p, make_vec(target, hit_record.p)};
        attenuation = m_albedo;
        return true;
    }

private:
    Vec3<T> m_albedo;
};

template <typename T>
class Metal : public Material<T>
{
public:
    Metal() = default;
    Metal(const Vec3<T>& a, T fuzz) :
        m_albedo{a},
        m_fuzz{fuzz}
    {}

    virtual bool scatter(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        const HitRecord<T>& hit_record,
        Vec3<T>& attenuation,
        Ray3<Point3<T>, Vec3<T>>& scattered,
        Rng& rng) const override
    {
        auto reflected = reflect(
             unit_vector(ray.direction()), hit_record.normal);
        scattered = {
            hit_record.p,
            reflected + m_fuzz * random_in_unit_sphere<T>(rng)
        };
        attenuation = m_albedo;
        return dot(scattered.direction(), hit_record.normal) > 0;
    }

private:
    Vec3<T> m_albedo;
    T m_fuzz;
};

template <typename T>
Vec3<T> refract(const Vec3<T>& v, const Vec3<T> n, T ni_over_nt)
{
    auto cosine = std::min(dot(-v, n), (T)1);
    Vec3<T> r_out_perp = ni_over_nt * (v + cosine * n);
    Vec3<T> r_out_parallel = -sqrt(abs(1 - r_out_perp.squared_length())) * n;
    return r_out_perp + r_out_parallel;
}

template <typename T>
T reflectance(T cosine, T ref_idx)
{
    // Schlick's approximation
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    return r0 * r0 + (1 - r0 * r0) * pow(1 - cosine, 5);
}

template <typename T>
class Dialectric : public Material<T>
{
public:
    Dialectric() = default;
    Dialectric(T ri) : m_refraction_index{ri} {}

    virtual bool scatter(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        const HitRecord<T>& hit_record,
        Vec3<T>& attenuation,
        Ray3<Point3<T>, Vec3<T>>& scattered,
        Rng& rng) const override
    {
        attenuation = {1, 1, 1};

        auto refraction_ratio = hit_record.front_face ?
            1 / m_refraction_index : m_refraction_index;

        auto unit_direction = unit_vector(ray.direction());

        auto cosine = std::min(dot(-unit_direction, hit_record.normal), (T)1);
        auto sine   = sqrt(1 - cosine * cosine);

        bool cannot_refract = refraction_ratio * sine > 1;

        Vec3<T> direction =
            cannot_refract || reflectance(cosine, refraction_ratio) > rng.random<T>() ?
            reflect(unit_direction, hit_record.normal) :
            refract(unit_direction, hit_record.normal, refraction_ratio);

        scattered = {hit_record.p, direction};

        return true;
    }

private:
    T m_refraction_index;
};
