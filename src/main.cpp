
#include "Image.hpp"
#include "Ray.hpp"
#include "Vec.hpp"
#include "Point.hpp"
#include "VecMath.hpp"
#include "Sphere.hpp"
#include "Hit.hpp"
#include "Camera.hpp"
#include "Material.hpp"

#include <array>
#include <limits>
#include <cstdio>
#include <random>

template <typename COLOR_TYPE>
struct PixelRGB
{
    COLOR_TYPE r;
    COLOR_TYPE g;
    COLOR_TYPE b;
};

using Vec = Vec3<float>;
using Point = Point3<float>;
using Ray = Ray3<Point, Vec>;
using Sphere = Sphere3<float>;

template <typename SPHERE_CONTAINER>
class World
{
public:
    World(SPHERE_CONTAINER& spheres)
        :
        m_spheres{spheres}
    {}

    template <typename T>
    bool is_hit(const Ray& ray, T t_min, T t_max, HitRecord<T>& record) const
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

std::mt19937 random_engine(time(0));

template <typename T, typename World>
Vec color(const Ray& ray, const World& world, int depth)
{
    HitRecord<T> hit_record;
    constexpr T t_min = 0.001;
    constexpr T t_max = std::numeric_limits<T>::max();
    if (world.is_hit(ray, t_min, t_max, hit_record))
    {
        Ray scattered;
        Vec attenuation;
        bool ray_was_scattered = hit_record.material->scatter(
            ray, hit_record, attenuation, scattered);
        if (depth < 50 && ray_was_scattered)
        {
            return attenuation * color<T, World>(scattered, world, depth+1);
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
    std::uniform_real_distribution<float> distr(0, 1);
    auto lookfrom = Point{13, 2, 3};
    auto lookat = Point{0, 0, 0};
    Camera<float> camera{
        random_engine,
        lookfrom,
        lookat,
        Vec{0, 1, 0},
        20,
        (float)image.width/image.height,
        0.1,
        10
    };
    auto num_samples = 100;

    for (int y = 0; y < image.height; ++y)
    {
        for (int x = 0; x < image.width; ++x)
        {
            Vec pixel_color{0, 0, 0};
            for (int sample = 0; sample < num_samples; ++sample)
            {
                const auto u = (x + distr(random_engine))/(image.width-1);
                const auto v = (y + distr(random_engine))/(image.height-1);
                const auto ray = camera.get_ray(u, v);

                pixel_color = pixel_color + color<float, World>(ray, world, 0);
            }
            pixel_color = pixel_color * (1.0f / num_samples);

            // gamma corrected
            pixel_color = Vec{
                (float)sqrt(pixel_color.x()),
                (float)sqrt(pixel_color.y()),
                (float)sqrt(pixel_color.z())
            };

            image[x][y] = {
                int(255.99f * pixel_color.x()),
                int(255.99f * pixel_color.y()),
                int(255.99f * pixel_color.z())
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

Lambertian<float> material_ground{Vec{0.5, 0.5, 0.5}};
Dialectric<float> material1{1.5};
Lambertian<float> material2{Vec{0.4, 0.2, 0.1}};
Metal<float> material3{Vec{0.7, 0.6, 0.5}, 0};


std::uniform_real_distribution<float> distr(0, 1);

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

std::array<Lambertian<float>, num_lamb> material_lamb;
std::array<Metal<float>, num_metal> material_metal;
std::array<Dialectric<float>, num_glass> material_glass;

World<decltype(spheres)> world{spheres};

int main()
{
    for (auto& mat : material_lamb)
    {
        mat = Lambertian<float>{Vec{
                distr(random_engine),
                distr(random_engine),
                distr(random_engine)}};
    }

    for (auto& mat : material_metal)
    {
        auto albedo = Vec{
            distr(random_engine)/2 + 0.5f,
            distr(random_engine)/2 + 0.5f,
            distr(random_engine)/2 + 0.5f
        };
        auto fuzz = distr(random_engine)/2;
        mat = Metal<float>{albedo, fuzz};
    }

    for (auto& mat : material_glass)
    {
        mat = Dialectric<float>{1.5};
    }

    int index = 4;
    for (int a = range_min; a < range_max; a++) {
        for (int b = range_min; b < range_max; b++) {
            auto choose_mat = distr(random_engine);
            Point center(a + 0.9*distr(random_engine),
                         0.2,
                         b + 0.9*distr(random_engine));

            if (make_vec(center, Point{4, 0.2, 0}).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto m = distr(random_engine) * num_lamb;
                    auto& material = material_lamb[(int)m];
                    spheres[index] = {center, 0.2, &material};
                } else if (choose_mat < 0.95) {
                    // metal
                    auto m = distr(random_engine) * num_metal;
                    auto& material = material_metal[(int)m];
                    spheres[index] = {center, 0.2, &material};
                } else {
                    // glass
                    auto m = distr(random_engine) * num_glass;
                    auto& material = material_glass[(int)m];
                    spheres[index] = {center, 0.2, &material};
                }
                index++;
            }
        }
    }

    generate_image(image, world);
    print_ppm_image(image);

    return 0;
}
