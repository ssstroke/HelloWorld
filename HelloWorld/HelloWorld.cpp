#include "RTWeekend.hpp"

#include "BVH.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"

#include <chrono>

using std::make_shared;
using std::shared_ptr;

int main()
{
    const auto start = std::chrono::high_resolution_clock::now();  // Start time

    HittableList world;

    auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RandomDouble();
            Point3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::Random() * Color::Random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    auto center2 = center + Vec3(0, RandomDouble(0, .5), 0);
                    world.Add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Color::Random(0.5, 1);
                    auto fuzz = RandomDouble(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    world = HittableList(make_shared<BVHNode>(world));

    Camera camera;

    camera.image_filename = "output/output_long_render_many_spheres_with_motion_blur_bvh.png";

    camera.image_width  = 400;
    camera.image_height = (size_t)((double)camera.image_width / (16.0 / 9.0));

    camera.samples_per_pixel = 100;
    camera.max_depth         = 50;

    camera.origin       = Point3(13, 2, 3);
    camera.direction    = UnitVector(-camera.origin);
    camera.direction_up = Vec3(0, 1, 0);
    camera.fov_vertical = 20;

    camera.defocus_angle  = 0.6;
    camera.focus_distance = 10;

    camera.Render(world);

    const auto end = std::chrono::high_resolution_clock::now();  // End time
    std::chrono::duration<double> elapsed = end - start;         // Compute duration

    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    return 0;
}
