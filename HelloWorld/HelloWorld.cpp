#include "RTWeekend.h"

#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

int main()
{
    HittableList world;
    
    const auto material_matte_0 = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    const auto material_matte_1 = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    const auto material_glass   = std::make_shared<Dielectric>(1.50);
    const auto material_metal   = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0);
    const auto material_bubble  = std::make_shared<Dielectric>(1.00 / 1.50);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_matte_0));
    world.Add(std::make_shared<Sphere>(Point3(0.0,  0.0,   -3.5), 0.5,   material_matte_1));
    world.Add(std::make_shared<Sphere>(Point3(-1.5, 0.0,   -1.0), 0.5,   material_glass));
    world.Add(std::make_shared<Sphere>(Point3(0.5,  0.0,   -1.0), 0.2,   material_bubble));
    world.Add(std::make_shared<Sphere>(Point3(1.5,  0.0,   -1.0), 0.5,   material_metal));
    world.Add(std::make_shared<Sphere>(Point3(0.0,  0.5,   -1.0), 0.5,   material_glass));

    /*auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(1.50);
    auto material_bubble = std::make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));*/

    Camera camera;
    camera.image_filename    = "output/output_camera_position_00_05_02_40_1.png";

    camera.image_width       = 512;
    camera.image_height      = (size_t)((double)camera.image_width / (4.0 / 3.0));

    camera.origin            = Point3(0, 0.5, 2);
    camera.v_fov             = 40;

    camera.samples_per_pixel = 64;
    camera.max_depth         = 32;

    camera.Render(world);

    return 0;
}
