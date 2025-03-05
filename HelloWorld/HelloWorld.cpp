#include "RTWeekend.hpp"

#include "Camera.hpp"
#include "Hittable.hpp"
#include "HittableList.hpp"
#include "Sphere.hpp"

int main()
{
    HittableList world;

    auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(1.50);
    auto material_bubble = std::make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));

    const auto center_0 = Point3(0.0, 0.0, -1.2);
    const auto center_1 = Point3(0.0, 0.4, -1.2);
    world.Add(std::make_shared<Sphere>(center_0, center_1, 0.5, material_center));

    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));

    Camera camera;
    camera.image_filename = "output/output_2025.png";

    camera.image_width  = 320;
    camera.image_height = 240;

    camera.origin     = Point3(0, 0, 2);
    //camera.direction  = UnitVector(-camera.origin);
    //camera.vup        = UnitVector(Vec3(1, 1, 0));
    camera.v_fov      = 50;

    /*camera.defocus_angle  = 0.6;
    camera.focus_distance = 10;*/

    camera.samples_per_pixel = 128;
    camera.max_depth         = 8;

    camera.Render(world);

    return 0;
}
