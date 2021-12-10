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
bool refract(const Vec3<T>& v, const Vec3<T> n, T ni_over_nt,
             Vec3<T>& refracted)
{
    auto uv = unit_vector(v);
    auto dt = dot(uv, n);
    auto discriminant = 1 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) -
            n * (T)sqrt(discriminant);
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
T schlick(float cosine, float ref_idx)
{
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

        auto dp = dot(ray.direction(), hit_record.normal);
        bool result = (dp > 0);

        auto outward_normal = result ? -hit_record.normal :
                                        hit_record.normal;
        auto ni_over_nt = result ? m_refraction_index :
                                   1 / m_refraction_index;
        auto cosine = result ?
            m_refraction_index * dp / ray.direction().length() :
            -dp / ray.direction().length();

        Vec3<T> refracted;

        if (refract(ray.direction(), outward_normal, ni_over_nt, refracted))
        {
            auto refract_prob = schlick<T>(cosine, m_refraction_index);

            if (rng.random<T>() < refract_prob)
            {
                auto reflected = reflect(ray.direction(), hit_record.normal);
                scattered = {hit_record.p, reflected};
            }
            else
            {
                scattered = {hit_record.p, refracted};
            }
        }
        else
        {
            auto reflected = reflect(ray.direction(), hit_record.normal);
            scattered = {hit_record.p, reflected};
        }
        return true;
    }

private:
    T m_refraction_index;
};
