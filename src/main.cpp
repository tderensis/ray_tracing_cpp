
#include "Image.hpp"
#include "Ray.hpp"
#include "Vec.hpp"
#include "Point.hpp"
#include "VecMath.hpp"
#include "Sphere.hpp"
#include "Hit.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Color.hpp"

#include <array>
#include <limits>
#include <cstdio>

template <typename COLOR_TYPE>
struct PixelRGB
{
    COLOR_TYPE r;
    COLOR_TYPE g;
    COLOR_TYPE b;
};

template <typename SPHERE_CONTAINER>
class World
{
public:
    World(SPHERE_CONTAINER& spheres)
        :
        m_spheres{spheres}
    {}

    template <typename T>
    bool is_hit(
        const Ray3<Point3<T>, Vec3<T>>& ray,
        T t_min,
        T t_max,
        HitRecord<T>& record) const
    {
        bool hit_anything = false;
        auto closest_so_far = t_max;
        for (const auto& sphere : m_spheres)
        {
            if (hit<T>(sphere, ray, t_min, closest_so_far, record))
            {
                hit_anything = true;
                closest_so_far = record.t;
            }
        }

        return hit_anything;
    }

private:
    SPHERE_CONTAINER& m_spheres;
};

using UnderlyingType = double;
using Vec = Vec3<UnderlyingType>;
using Point = Point3<UnderlyingType>;
using Ray = Ray3<Point, Vec>;
using Sphere = Sphere3<UnderlyingType>;
using LambertianMat = Lambertian<UnderlyingType>;
using DialectricMat = Dialectric<UnderlyingType>;
using MetalMat = Metal<UnderlyingType>;
using PixelColor = Color<UnderlyingType>;

Rng rng{(unsigned)time(0)};

constexpr int depth = 50;
constexpr int num_samples_per_pixel = 100;

LambertianMat material_ground{Vec{0.5, 0.5, 0.5}};
DialectricMat material1{1.5};
LambertianMat material2{Vec{0.4, 0.2, 0.1}};
MetalMat      material3{Vec{0.7, 0.6, 0.5}, 0};

constexpr auto range_min = -11;
constexpr auto range_max = 11;
constexpr int num_random = (range_max - range_min)*(range_max-range_min);
constexpr int num_lamb = num_random * 0.8;
constexpr int num_metal = num_random * 0.15;
constexpr int num_glass = num_random - num_lamb - num_metal;

std::array<Sphere, 4+num_random> spheres = {
    Sphere{ Point{0, -1000, 0}, 1000, &material_ground },
    Sphere{ Point{ 0, 1, 0}, 1,       &material1 },
    Sphere{ Point{-4, 1, 0}, 1,       &material2 },
    Sphere{ Point{ 4, 1, 0}, 1,       &material3 },
};

std::array<LambertianMat, num_lamb>  material_lamb;
std::array<MetalMat,      num_metal> material_metal;
std::array<DialectricMat, num_glass> material_glass;

World<decltype(spheres)> world{spheres};

void generate_world()
{
    for (auto& mat : material_lamb)
    {
        mat = {Vec{
                rng.random<UnderlyingType>(),
                rng.random<UnderlyingType>(),
                rng.random<UnderlyingType>()}};
    }

    for (auto& mat : material_metal)
    {
        auto albedo = Vec{
            rng.random<UnderlyingType>(0.5, 1),
            rng.random<UnderlyingType>(0.5, 1),
            rng.random<UnderlyingType>(0.5, 1)
        };
        auto fuzz = rng.random<UnderlyingType>(0, 0.5);
        mat = {albedo, fuzz};
    }

    for (auto& mat : material_glass)
    {
        mat = {1.5};
    }

    int index = 4;
    for (int a = range_min; a < range_max; a++) {
        for (int b = range_min; b < range_max; b++) {
            auto choose_mat = rng.random<UnderlyingType>();
            Point center(a + 0.9 * rng.random<UnderlyingType>(),
                         0.2,
                         b + 0.9 * rng.random<UnderlyingType>());

            if (make_vec(center, Point{4, 0.2, 0}).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto m = rng.random<UnderlyingType>() * num_lamb;
                    auto& material = material_lamb[(int)m];
                    spheres[index] = {center, 0.2, &material};
                } else if (choose_mat < 0.95) {
                    // metal
                    auto m = rng.random<UnderlyingType>() * num_metal;
                    auto& material = material_metal[(int)m];
                    spheres[index] = {center, 0.2, &material};
                } else {
                    // glass
                    auto m = rng.random<UnderlyingType>() * num_glass;
                    auto& material = material_glass[(int)m];
                    spheres[index] = {center, 0.2, &material};
                }
                index++;
            }
        }
    }
}

template <typename T, typename World>
PixelColor color(const Ray& ray, const World& world, int depth)
{
    HitRecord<T> hit_record;
    constexpr T t_min = 0.001;
    constexpr T t_max = std::numeric_limits<T>::max();
    if (world.is_hit(ray, t_min, t_max, hit_record))
    {
        Ray scattered;
        Vec attenuation;
        bool ray_was_scattered = hit_record.material->scatter(
            ray, hit_record, attenuation, scattered, rng);

        if (depth != 0 && ray_was_scattered)
        {
            auto pixel_color = color<T, World>(scattered, world, depth-1);
            return attenuation * pixel_color;
        }
        else
        {
            return {0, 0, 0};
        }
    }
    else
    {
        auto unit_direction = unit_vector(ray.direction());
        auto t = (unit_direction.y() + 1) / 2;
        auto result = (1 - t) * Vec{1.0, 1.0, 1.0} +
                           t  * Vec{0.5, 0.7, 1.0};
        return {result.x(), result.y(), result.z()};
    }
}

template <typename Image, typename World>
void generate_image(Image& image, const World& world)
{
    auto lookfrom = Point{13, 2, 3};
    auto lookat = Point{0, 0, 0};

    Camera<UnderlyingType> camera{
        lookfrom,
        lookat,
        Vec{0, 1, 0},
        20,
        (UnderlyingType)image.width/image.height,
        0.1,
        10
    };

    for (int y = 0; y < image.height; ++y)
    {
        for (int x = 0; x < image.width; ++x)
        {
            PixelColor pixel_color{0, 0, 0};
            for (int sample = 0; sample < num_samples_per_pixel; ++sample)
            {
                const auto u = (x + rng.random<UnderlyingType>())/(image.width-1);
                const auto v = (y + rng.random<UnderlyingType>())/(image.height-1);
                const auto ray = camera.get_ray(u, v, rng);

                pixel_color = pixel_color + color<UnderlyingType, World>(ray, world, depth);
            }
            pixel_color = pixel_color / (UnderlyingType) num_samples_per_pixel;

            pixel_color = gamma_correct(pixel_color);

            image[x][y] = {
                int(255.99f * pixel_color.r()),
                int(255.99f * pixel_color.g()),
                int(255.99f * pixel_color.b())
            };
        }
    }
}

template <typename Image>
void print_ppm_image(const Image& image)
{
    // Print the image header first
    printf("P3\n");
    printf("%d %d\n", image.width, image.height);
    printf("255\n");

    for (int y = image.height-1; y >= 0; --y)
    {
        for (int x = 0; x < image.width; ++x)
        {
            auto pixel = image[x][y];
            printf("%d %d %d\n", pixel.r, pixel.g, pixel.b);
        }
    }
}

constexpr auto aspect_ratio = 16.0/9.0;
constexpr int image_width = 400;
constexpr int image_height = image_width / aspect_ratio;

Image<PixelRGB<int>, image_width, image_height> image;

int main()
{
    generate_world();
    generate_image(image, world);
    print_ppm_image(image);

    return 0;
}
