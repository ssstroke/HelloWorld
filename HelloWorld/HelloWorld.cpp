#include "RTWeekend.hpp"

#include "Hit_ConstantMedium.hpp"

#include "Camera.hpp"
#include "Color.hpp"
#include "Hittable.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Util.hpp"
#include "Vec3.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <tinyfiledialogs.h>

using std::make_shared;
using std::shared_ptr;

struct CameraSettings {
    float fov = 35.0f;
    float rotation = 0.0f;
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float direction[3] = { 0.0f, 0.0f, -1.0f };
    int width = 576;
    int height = 384;
    int samples = 4;
    int bounces = 2;
};

static std::string image_filename;

//static void BouncingSpheres()
//{
//    Hit_List world;
//
//    const auto checker = make_shared<Tex_Checker>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
//    world.Add(make_shared<Hit_Sphere>(Point3(0, -1000, 0), 1000, make_shared<Mat_Lambertian>(checker)));
//
//    for (int a = -11; a < 11; a++) {
//        for (int b = -11; b < 11; b++) {
//            const auto choose_mat = RandomDouble();
//            const Point3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());
//
//            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
//                if (choose_mat < 0.8) {
//                    // diffuse
//                    const auto albedo = Color::Random() * Color::Random();
//                    const auto sphere_material = make_shared<Mat_Lambertian>(albedo);
//                    const auto center2 = center + Vec3(0, RandomDouble(0, .5), 0);
//                    world.Add(make_shared<Hit_Sphere>(center, center2, 0.2, sphere_material));
//                }
//                else if (choose_mat < 0.95) {
//                    // metal
//                    const auto albedo = Color::Random(0.5, 1);
//                    const auto fuzz = RandomDouble(0, 0.5);
//                    const auto sphere_material = make_shared<Mat_Metal>(albedo, fuzz);
//                    world.Add(make_shared<Hit_Sphere>(center, 0.2, sphere_material));
//                }
//                else {
//                    // glass
//                    const auto sphere_material = make_shared<Mat_Dielectric>(1.5);
//                    world.Add(make_shared<Hit_Sphere>(center, 0.2, sphere_material));
//                }
//            }
//        }
//    }
//
//    const auto material1 = make_shared<Mat_Dielectric>(1.5);
//    world.Add(make_shared<Hit_Sphere>(Point3(0, 1, 0), 1.0, material1));
//
//    const auto material2 = make_shared<Mat_Lambertian>(Color(0.4, 0.2, 0.1));
//    world.Add(make_shared<Hit_Sphere>(Point3(-4, 1, 0), 1.0, material2));
//
//    const auto material3 = make_shared<Mat_Metal>(Color(0.7, 0.6, 0.5), 0.0);
//    world.Add(make_shared<Hit_Sphere>(Point3(4, 1, 0), 1.0, material3));
//
//    world = Hit_List(make_shared<Hit_BVHNode>(world));
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width  = 640;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 96;
//    camera.max_depth         = 32;
//    camera.origin       = Point3(13, 2, 3);
//    camera.direction    = UnitVector(-camera.origin);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.fov_vertical = 20;
//    camera.defocus_angle  = 0.6;
//    camera.focus_distance = 10;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void CheckeredSpheres()
//{
//    Hit_List world;
//
//    const auto checker = make_shared<Tex_Checker>(0.48, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
//
//    world.Add(make_shared<Hit_Sphere>(Point3(0, -10, 0), 10, make_shared<Mat_Lambertian>(checker)));
//    world.Add(make_shared<Hit_Sphere>(Point3(0,  10, 0), 10, make_shared<Mat_Lambertian>(checker)));
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 400;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth = 32;
//    camera.origin = Point3(13, 2, 3);
//    camera.direction = UnitVector(-camera.origin);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.fov_vertical = 20;
//    camera.defocus_angle = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void Earth()
//{
//    Hit_List world;
//
//    const auto tex_earth = make_shared<Tex_Image>("assets/ai_vaporwave_.png");
//    const auto mat_earth = make_shared<Mat_Lambertian>(tex_earth);
//    const auto hit_earth = make_shared<Hit_Sphere>(Point3(0, 0, 0), 2, mat_earth);
//    world.Add(hit_earth);
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 400;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth = 32;
//    camera.origin = Point3(4, 3, 12);
//    camera.direction = UnitVector(-camera.origin);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.fov_vertical = 20;
//    camera.defocus_angle = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void PerlinSpheres()
//{
//    Hit_List world;
//
//    const auto tex_perlin = make_shared<Tex_Perlin>(4);
//    const auto mat_perlin = make_shared<Mat_Lambertian>(tex_perlin);
//    const auto hit_ground = make_shared<Hit_Sphere>(Point3(0, -1000, 0), 1000, mat_perlin);
//    const auto hit_sphere = make_shared<Hit_Sphere>(Point3(0, 2, 0), 2, mat_perlin);
//
//    world.Add(hit_ground);
//    world.Add(hit_sphere);
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 400;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth = 32;
//    camera.origin = Point3(4, 3, 12);
//    camera.direction = UnitVector(-camera.origin);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.fov_vertical = 20;
//    camera.defocus_angle = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void Quads()
//{
//    Hit_List world;
//
//    shared_ptr<Mat_Lambertian> left_red     = make_shared<Mat_Lambertian>(Color(1.0, 0.2, 0.2));
//    shared_ptr<Mat_Lambertian> back_green   = make_shared<Mat_Lambertian>(Color(0.2, 1.0, 0.2));
//    shared_ptr<Mat_Lambertian> right_blue   = make_shared<Mat_Lambertian>(Color(0.2, 0.2, 1.0));
//    shared_ptr<Mat_Lambertian> upper_orange = make_shared<Mat_Lambertian>(Color(1.0, 0.5, 0.0));
//    shared_ptr<Mat_Lambertian> lower_teal   = make_shared<Mat_Lambertian>(Color(0.2, 0.8, 0.8));
//
//    world.Add(make_shared<Hit_Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0),  left_red));
//    world.Add(make_shared<Hit_Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0),  Vec3(0, 4, 0),  back_green));
//    world.Add(make_shared<Hit_Quad>(Point3(3, -2, 1),  Vec3(0, 0, 4),  Vec3(0, 4, 0),  right_blue));
//    world.Add(make_shared<Hit_Quad>(Point3(-2, 3, 1),  Vec3(4, 0, 0),  Vec3(0, 0, 4),  upper_orange));
//    world.Add(make_shared<Hit_Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0),  Vec3(0, 0, -4), lower_teal));
//
//    Camera camera;
//    camera.image_filename    = image_filename;
//    camera.image_width       = 640;
//    camera.image_height      = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth         = 32;
//    camera.fov_vertical      = 80;
//    camera.origin            = Point3(0, 0, 9);
//    camera.direction         = UnitVector(-camera.origin);
//    camera.direction_up      = Vec3(0, 1, 0);
//    camera.defocus_angle     = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void Tris()
//{
//    Hit_List world;
//
//    shared_ptr<Mat_Lambertian> left_red = make_shared<Mat_Lambertian>(Color(1.0, 0.2, 0.2));
//    shared_ptr<Mat_Lambertian> back_green = make_shared<Mat_Lambertian>(Color(0.2, 1.0, 0.2));
//    shared_ptr<Mat_Lambertian> right_blue = make_shared<Mat_Lambertian>(Color(0.2, 0.2, 1.0));
//    shared_ptr<Mat_Lambertian> upper_orange = make_shared<Mat_Lambertian>(Color(1.0, 0.5, 0.0));
//    shared_ptr<Mat_Lambertian> lower_teal = make_shared<Mat_Lambertian>(Color(0.2, 0.8, 0.8));
//
//    world.Add(make_shared<Hit_Tri>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), left_red));
//    world.Add(make_shared<Hit_Tri>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
//    world.Add(make_shared<Hit_Tri>(Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
//    world.Add(make_shared<Hit_Tri>(Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
//    world.Add(make_shared<Hit_Tri>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lower_teal));
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 640;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth = 32;
//    camera.fov_vertical = 80;
//    camera.origin = Point3(0, 0, 9);
//    camera.direction = UnitVector(-camera.origin);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.defocus_angle = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}
//
//static void Emission()
//{
//    Hit_List world;
//
//    const auto tex_perlin = make_shared<Tex_Perlin>(4);
//    world.Add(make_shared<Hit_Sphere>(Point3(0, -1000, 0), 1000, make_shared<Mat_Lambertian>(tex_perlin)));
//    world.Add(make_shared<Hit_Sphere>(Point3(0, 2, 0), 2, make_shared<Mat_Lambertian>(tex_perlin)));
//
//    const auto mat_diffuseLight = make_shared<Mat_DiffuseLight>(Color(4, 4, 4));
//    world.Add(make_shared<Hit_Sphere>(Point3(0, 7, 0), 2, mat_diffuseLight));
//    world.Add(make_shared<Hit_Quad>(Point3(3, 1, -2), Vec3(2, 0, 0), Vec3(0, 2, 0), mat_diffuseLight));
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 640;
//    camera.image_height = int(double(camera.image_width) / (16.0 / 9.0));
//    camera.samples_per_pixel = 64;
//    camera.max_depth = 32;
//    camera.fov_vertical = 20;
//    camera.origin = Point3(26, 3, 6);
//    camera.direction = UnitVector(-camera.origin + Vec3(0, 2, 0));
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.defocus_angle = 0;
//    camera.background = Color(0, 0, 0);
//    camera.Render(world);
//}
//
//static void CornellBox()
//{
//    Hit_List world;
//
//    const auto mat_red   = make_shared<Mat_Lambertian>(Color(.65, .05, .05));
//    const auto mat_white = make_shared<Mat_Lambertian>(Color(.73, .73, .73));
//    const auto mat_green = make_shared<Mat_Lambertian>(Color(.12, .45, .15));
//    const auto mat_light = make_shared<Mat_DiffuseLight>(Color(16, 16, 16));
//
//    world.Add(make_shared<Hit_Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), mat_green));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), mat_red));
//    world.Add(make_shared<Hit_Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), mat_light));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), mat_white));
//    world.Add(make_shared<Hit_Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), mat_white));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), mat_white));
//
//    shared_ptr<Hittable> box_left = Box(Point3(0, 0, 0), Point3(165, 330, 165), mat_white);
//    box_left = make_shared<Hit_RotateY>(box_left, 15);
//    box_left = make_shared<Hit_Translate>(box_left, Vec3(265, 0, 295));
//    world.Add(box_left);
//
//    shared_ptr<Hittable> box_right = Box(Point3(0, 0, 0), Point3(165, 165, 165), mat_white);
//    box_right = make_shared<Hit_RotateY>(box_right, -18);
//    box_right = make_shared<Hit_Translate>(box_right, Vec3(130, 0, 32));
//    world.Add(box_right);
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 640;
//    camera.image_height = 640;
//    camera.samples_per_pixel = 256;
//    camera.max_depth = 16;
//    camera.fov_vertical = 40;
//    camera.origin = Point3(278, 278, -800);
//    camera.direction = Vec3(0, 0, 1);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.defocus_angle = 0;
//    camera.background = Color(0, 0, 0);
//    camera.Render(world);
//}
//
//static void CornellSmoke()
//{
//    Hit_List world;
//
//    const auto mat_red = make_shared<Mat_Lambertian>(Color(.65, .05, .05));
//    const auto mat_white = make_shared<Mat_Lambertian>(Color(.73, .73, .73));
//    const auto mat_green = make_shared<Mat_Lambertian>(Color(.12, .45, .15));
//    const auto mat_light = make_shared<Mat_DiffuseLight>(Color(8, 8, 8));
//
//    world.Add(make_shared<Hit_Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), mat_green));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), mat_red));
//    world.Add(make_shared<Hit_Quad>(Point3(113, 554, 127), Vec3(330, 0, 0), Vec3(0, 0, 305), mat_light));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), mat_white));
//    world.Add(make_shared<Hit_Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), mat_white));
//    world.Add(make_shared<Hit_Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), mat_white));
//
//    shared_ptr<Hittable> box_left = Box(Point3(0, 0, 0), Point3(165, 330, 165), mat_white);
//    box_left = make_shared<Hit_RotateY>(box_left, 15);
//    box_left = make_shared<Hit_Translate>(box_left, Vec3(265, 0, 295));
//    world.Add(make_shared<Hit_ConstantMedium>(box_left, 0.01, Color(0, 0, 0)));
//
//    shared_ptr<Hittable> box_right = Box(Point3(0, 0, 0), Point3(165, 165, 165), mat_white);
//    box_right = make_shared<Hit_RotateY>(box_right, -18);
//    box_right = make_shared<Hit_Translate>(box_right, Vec3(130, 0, 32));
//    world.Add(make_shared<Hit_ConstantMedium>(box_right, 0.01, Color(1, 1, 1)));
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 360;
//    camera.image_height = 360;
//    camera.samples_per_pixel = 32;
//    camera.max_depth = 50;
//    camera.fov_vertical = 40;
//    camera.origin = Point3(278, 278, -800);
//    camera.direction = Vec3(0, 0, 1);
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.defocus_angle = 0;
//    camera.background = Color(0, 0, 0);
//    camera.Render(world);
//}

//static void ImportMesh(const std::string& name)
//{
//    Hit_List world = MeshLoad(name);
//
//    Camera camera;
//    camera.image_filename = image_filename;
//    camera.image_width = 640;
//    camera.image_height = 480;
//    camera.samples_per_pixel = 32;
//    camera.max_depth = 4;
//    camera.fov_vertical = 35;
//    camera.origin = Point3(0, 0, 0);
//    camera.direction = UnitVector(Point3(0, 0, -1));
//    camera.direction_up = Vec3(0, 1, 0);
//    camera.defocus_angle = 0;
//    camera.background = Color(0.7, 0.8, 1);
//    camera.Render(world);
//}

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 640, 480, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    if (renderer == nullptr)
    {
        SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    CameraSettings settings;

    SDL_Texture* texture = nullptr;
    Camera camera;

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            // Create a window called "Hello, world!" and append into it.
            ImGui::Begin("Hello, world!");

            ImGui::SliderFloat("FOV", &settings.fov, 1.0f, 180.0f, "%.1f deg");
            ImGui::SliderFloat("Rotation", &settings.rotation, -180.0f, 180.0f, "%.1f deg");

            ImGui::InputFloat3("Position", settings.position);
            ImGui::InputFloat3("Direction", settings.direction);

            ImGui::SliderInt("Width", &settings.width, 1, 1280);
            ImGui::SliderInt("Height", &settings.height, 1, 720);

            ImGui::SliderInt("Samples", &settings.samples, 1, 256);
            ImGui::SliderInt("Bounces", &settings.bounces, 1, 32);

            if (ImGui::Button("Render"))
            {
                if (texture != nullptr)
                {
                    SDL_free(texture);
                }

                SDL_SetWindowSize(window, settings.width, settings.height);
                SDL_SetWindowPosition(window, 32, 32);

                // Start rendering.
                camera.image_width = settings.width;
                camera.image_height = settings.height;
                camera.samples_per_pixel = settings.samples;
                camera.max_depth = settings.bounces;
                camera.fov_vertical = settings.fov;
                camera.origin = Point3(settings.position[0], settings.position[1], settings.position[2]);
                camera.direction = UnitVector(Vec3(settings.direction[0], settings.direction[1], settings.direction[2]));
                camera.direction_up = Vec3(0, 1, 0);
                camera.defocus_angle = 0;
                camera.background = Color(0.7, 0.8, 1.0);

                Hit_List world = MeshLoad("assets/cube.obj");
                camera.Render(world, renderer);

                texture = camera.texture;
            }

            if (texture != nullptr)
            {
                if (ImGui::Button("Save as..."))
                {
                    char const* lFilterPatterns[2] = { "*.png" };

                    const char* save_path = tinyfd_saveFileDialog(
                        "Save image as...",
                        "render.png",
                        1,
                        lFilterPatterns,
                        NULL
                    );

                    if (save_path)
                    {
                        // Write to file.
                        SDL_Log(save_path);

                        if (camera.surface != nullptr)
                        {
                            if (IMG_SavePNG(camera.surface, save_path) == false)
                            {
                                SDL_Log(SDL_GetError());
                            }
                        }
                    }
                }
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);

        if (texture == nullptr)
        {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, SDL_ALPHA_OPAQUE);
            SDL_RenderClear(renderer);
        }
        else
        {
            SDL_RenderTexture(renderer, texture, NULL, NULL);
        }

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
