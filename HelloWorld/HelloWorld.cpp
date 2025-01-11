#include "RTWeekend.h"

#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Sphere.h"

int main()
{
    HittableList world;
    
    /*const auto material_matte_0 = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    const auto material_matte_1 = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    const auto material_glass   = std::make_shared<Dielectric>(1.50);
    const auto material_metal_0 = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0);
    const auto material_metal_1 = std::make_shared<Metal>(Color(0.5, 0.5, 0.5), 0);
    const auto material_bubble  = std::make_shared<Dielectric>(1.00 / 1.50);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_metal_1));
    world.Add(std::make_shared<Sphere>(Point3(0.0,  0.0,   -3.5), 0.5,   material_matte_1));
    world.Add(std::make_shared<Sphere>(Point3(-1.5, 0.0,   -1.0), 0.5,   material_glass));
    world.Add(std::make_shared<Sphere>(Point3(0.5,  0.0,   -1.0), 0.2,   material_bubble));
    world.Add(std::make_shared<Sphere>(Point3(1.5,  0.0,   -1.0), 0.5,   material_metal_0));
    world.Add(std::make_shared<Sphere>(Point3(0.0,  0.5,   -1.0), 0.5,   material_glass));*/

    auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(1.50);
    auto material_bubble = std::make_shared<Dielectric>(1.00 / 1.50);
    auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.0);

    world.Add(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));

    //auto ground_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    //world.Add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    //for (int a = -11; a < 11; a++) {
    //    for (int b = -11; b < 11; b++) {
    //        auto choose_mat = RandomDouble();
    //        Point3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

    //        if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
    //            std::shared_ptr<Material> sphere_material;

    //            if (choose_mat < 0.8) {
    //                // diffuse
    //                auto albedo = Color(RandomDouble(), RandomDouble(), RandomDouble()) * Color(RandomDouble(), RandomDouble(), RandomDouble());
    //                sphere_material = std::make_shared<Lambertian>(albedo);
    //                world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //            }
    //            else if (choose_mat < 0.95) {
    //                // metal
    //                auto albedo = Color(RandomDouble(0.5, 1), RandomDouble(0.5, 1), RandomDouble(0.5, 1));
    //                auto fuzz = RandomDouble(0, 0.5);
    //                sphere_material = std::make_shared<Metal>(albedo, fuzz);
    //                world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //            }
    //            else {
    //                // glass
    //                sphere_material = std::make_shared<Dielectric>(1.5);
    //                world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
    //            }
    //        }
    //    }
    //}

    //auto material1 = std::make_shared<Dielectric>(1.5);
    //world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    //auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    //world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    //auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    //world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    Camera camera;
    camera.image_filename = "output/output_30_12_24_1.png";

    camera.image_width  = 1024;
    camera.image_height = 768;

    //camera.origin     = Point3(13, 2, 3);
    //camera.direction  = UnitVector(-camera.origin);
    //camera.vup        = UnitVector(Vec3(1, 1, 0));
    camera.v_fov      = 20;

    /*camera.defocus_angle  = 0.6;
    camera.focus_distance = 10;*/

    camera.samples_per_pixel = 16;
    camera.max_depth         = 16;

    camera.Render(world);

    return 0;
}
