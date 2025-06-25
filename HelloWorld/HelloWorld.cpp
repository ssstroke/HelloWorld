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

struct CameraSettings
{
    float fov = 40.0f;
    float rotation = 0.0f;
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float direction[3] = { 0.0f, -0.2f, -1.0f };
    int width = 384;
    int height = 384;
    int samples = 2;
    int bounces = 2;
};

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Ray tracer", 640, 480, window_flags);
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

    char* obj_path = nullptr;

    long long duration = 0;

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
            ImGui::Begin("Render settings");

            if (ImGui::Button("Open .obj file..."))
            {
                char const* lFilterPatterns[1] = { "*.obj" };

                obj_path = tinyfd_openFileDialog(
                    "Select an .obj file",
                    "",
                    1,
                    lFilterPatterns,
                    NULL,
                    0
                );
            }

            ImGui::SliderFloat("FOV", &settings.fov, 1.0f, 180.0f, "%.1f deg");
            ImGui::SliderFloat("Rotation", &settings.rotation, -180.0f, 180.0f, "%.1f deg");

            ImGui::InputFloat3("Position", settings.position);
            ImGui::InputFloat3("Direction", settings.direction);

            ImGui::SliderInt("Width", &settings.width, 1, 1000);
            ImGui::SliderInt("Height", &settings.height, 1, 1000);

            ImGui::SliderInt("Samples", &settings.samples, 1, 128);
            ImGui::SliderInt("Bounces", &settings.bounces, 1, 32);

            if (ImGui::Button("Render") && obj_path)
            {
                // Start timing
                const auto start = std::chrono::high_resolution_clock::now();

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
                camera.background = Color(255.0 / 255.0, 242 / 255.0, 202.0 / 255.0);

                Hit_List world = MeshLoad(std::string(obj_path));

                camera.Render(world, renderer);

                texture = camera.texture;

                // End timing
                const auto end = std::chrono::high_resolution_clock::now();

                // Calculate duration in microseconds
                duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            }

            if (texture != nullptr)
            {
                if (ImGui::Button("Save as..."))
                {
                    char const* lFilterPatterns[1] = { "*.png" };

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

            ImGui::Text("Render time: %lld seconds", duration);

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
