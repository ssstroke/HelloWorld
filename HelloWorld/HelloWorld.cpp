#include "Camera.hpp"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"
#include "RTWeekend.hpp"
#include "Texture.hpp"
#include "Vec3.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

using std::make_shared;
using std::shared_ptr;

static std::string image_filename;

static void BouncingSpheres()
{
    Hit_List world;

    const auto checker = make_shared<Tex_Checker>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    world.Add(make_shared<Hit_Sphere>(Point3(0, -1000, 0), 1000, make_shared<Mat_Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const auto choose_mat = RandomDouble();
            const Point3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    const auto albedo = Color::Random() * Color::Random();
                    const auto sphere_material = make_shared<Mat_Lambertian>(albedo);
                    const auto center2 = center + Vec3(0, RandomDouble(0, .5), 0);
                    world.Add(make_shared<Hit_Sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    const auto albedo = Color::Random(0.5, 1);
                    const auto fuzz = RandomDouble(0, 0.5);
                    const auto sphere_material = make_shared<Mat_Metal>(albedo, fuzz);
                    world.Add(make_shared<Hit_Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    const auto sphere_material = make_shared<Mat_Dielectric>(1.5);
                    world.Add(make_shared<Hit_Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    const auto material1 = make_shared<Mat_Dielectric>(1.5);
    world.Add(make_shared<Hit_Sphere>(Point3(0, 1, 0), 1.0, material1));

    const auto material2 = make_shared<Mat_Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(make_shared<Hit_Sphere>(Point3(-4, 1, 0), 1.0, material2));

    const auto material3 = make_shared<Mat_Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Hit_Sphere>(Point3(4, 1, 0), 1.0, material3));

    world = Hit_List(make_shared<Hit_BVHNode>(world));

    Camera camera;

    camera.image_filename = image_filename;

    camera.image_width  = 640;
    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));

    camera.samples_per_pixel = 96;
    camera.max_depth         = 32;

    camera.origin       = Point3(13, 2, 3);
    camera.direction    = UnitVector(-camera.origin);
    camera.direction_up = Vec3(0, 1, 0);
    camera.fov_vertical = 20;

    camera.defocus_angle  = 0.6;
    camera.focus_distance = 10;

    camera.Render(world);
}

static void CheckeredSpheres()
{
    Hit_List world;

    const auto checker = make_shared<Tex_Checker>(0.48, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    world.Add(make_shared<Hit_Sphere>(Point3(0, -10, 0), 10, make_shared<Mat_Lambertian>(checker)));
    world.Add(make_shared<Hit_Sphere>(Point3(0,  10, 0), 10, make_shared<Mat_Lambertian>(checker)));

    Camera camera;

    camera.image_filename = image_filename;

    camera.image_width = 400;
    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));

    camera.samples_per_pixel = 64;
    camera.max_depth = 32;

    camera.origin = Point3(13, 2, 3);
    camera.direction = UnitVector(-camera.origin);
    camera.direction_up = Vec3(0, 1, 0);
    camera.fov_vertical = 20;

    camera.defocus_angle = 0;

    camera.Render(world);
}

static void Earth()
{
    Hit_List world;

    const auto tex_earth = make_shared<Tex_Image>("assets/zloisham.jpg");
    const auto mat_earth = make_shared<Mat_Lambertian>(tex_earth);
    const auto hit_earth = make_shared<Hit_Sphere>(Point3(0, 0, 0), 2, mat_earth);
    world.Add(hit_earth);

    const auto material3 = make_shared<Mat_Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Hit_Sphere>(Point3(4, 0, -2), 2, material3));

    Camera camera;

    camera.image_filename = image_filename;

    camera.image_width = 400;
    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));

    camera.samples_per_pixel = 64;
    camera.max_depth = 32;

    camera.origin = Point3(4, 3, 12);
    camera.direction = UnitVector(-camera.origin);
    camera.direction_up = Vec3(0, 1, 0);
    camera.fov_vertical = 20;

    camera.defocus_angle = 0;

    camera.Render(world);
}

static void PerlinSpheres()
{
    Hit_List world;

    const auto tex_perlin = make_shared<Tex_Perlin>(4);
    const auto mat_perlin = make_shared<Mat_Lambertian>(tex_perlin);
    const auto hit_ground = make_shared<Hit_Sphere>(Point3(0, -1000, 0), 1000, mat_perlin);
    const auto hit_sphere = make_shared<Hit_Sphere>(Point3(0, 2, 0), 2, mat_perlin);

    world.Add(hit_ground);
    world.Add(hit_sphere);

    Camera camera;

    camera.image_filename = image_filename;

    camera.image_width = 400;
    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));

    camera.samples_per_pixel = 64;
    camera.max_depth = 32;

    camera.origin = Point3(4, 3, 12);
    camera.direction = UnitVector(-camera.origin);
    camera.direction_up = Vec3(0, 1, 0);
    camera.fov_vertical = 20;

    camera.defocus_angle = 0;

    camera.Render(world);
}

int main()
{
    const auto start = std::chrono::high_resolution_clock::now();

    // Change switch case!
    image_filename = "output/perlin_vecs-on-lattice-points.png";

    // Change image_filename!
    switch (4)
    {
    case 1: BouncingSpheres();
        break;
    case 2: CheckeredSpheres();
        break;
    case 3: Earth();
        break;
    case 4: PerlinSpheres();
        break;
    }

    std::chrono::duration<double> elapsed = 
        std::chrono::high_resolution_clock::now() - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    return 0;
}
