﻿// ↑ ↓ → ←
//
// I use right-handed coordinates (still can't understand why are they called like that lol).
// X-axis goes right, Y-axis goes up, negative Z-axis points in the viewing direction.

#pragma once

#include "Hittable.hpp"
#include "Material.hpp"
#include "Image.hpp"
#include "Ray.hpp"

#include <iostream>
#include <string>

class Camera
{
public:
    std::string image_filename = "output.png";

    size_t image_width       = 100;              // Rendered image width in pixels
    size_t image_height      = 100;              // Rendered image height in pixels

    Point3 origin            = Point3(0, 0, 0);
    Vec3   direction         = Vec3(0, 0, -1);
    Vec3   vup               = Vec3(0, 1, 0);    // Camera-relative "up" direction
    double v_fov             = 90;               // Vertical view angle (field of view)

    double defocus_angle     = 0;                // Variation angle of rays through each pixel
    double focus_distance    = 10;               // Distance from camera origin to plane of perfect focus

    size_t samples_per_pixel = 10;               // Count of random samples for each pixel
    size_t max_depth         = 10;               // Maximum number of ray bounces into scene

    Camera() {}

    void Render(const Hittable& world)
    {
        this->Initialize();

        for (size_t h = 0; h < this->image_height; ++h)
        {
            std::clog << "Scanline " << h << " out of " << this->image_height - 1 << ".\n";
            for (size_t w = 0; w < this->image_width; ++w)
            {
                Color pixel_color(0, 0, 0);

                for (size_t sample = 0; sample < this->samples_per_pixel; ++sample)
                {
                    const auto ray = this->GetRay(w, h);
                    pixel_color += RayColor(ray, this->max_depth, world);
                }

                image.WriteColor(w, h, this->pixel_samples_scale * pixel_color);
            }
        }

        if (image.WritePNG(this->image_filename))
        {
            std::clog << "Write to " << this->image_filename << ".";
        }
        else
        {
            std::clog << "Failed to write to " << this->image_filename << ".";
        }
    }
private:
    double aspect_ratio = 1.0;             // Ratio of image width over height

    Point3 pixel00_location;               // Location of pixel (0, 0)
    Vec3   pixel_delta_u;                  // Offset to pixel to the right
    Vec3   pixel_delta_v;                  // Offset to pixel to the bottom

    double pixel_samples_scale = 1;        // Color scale factor for a sum of pixel samples

    Image  image = Image(this->image_width, this->image_height);
     
    Vec3   u, v, w;                        // Camera frame basis vectors (right, up, opposite view direction)

    Vec3   defocus_disk_u, defocus_disk_v; // Defocus disk horizontal, vertical radius

    void Initialize()
    {
        this->image = Image(this->image_width, this->image_height);

        this->pixel_samples_scale = 1.0 / samples_per_pixel;

        this->aspect_ratio = (double)this->image_width / (double)this->image_height;

        // Viewport dimensions.
        const auto theta = DegreesToRadians(this->v_fov);
        const auto h = std::tan(theta / 2);
        const auto viewport_height = 2 * h * this->focus_distance;
        const auto viewport_width = viewport_height * this->aspect_ratio;

        // Calculate u, v, w.
        this->w = -this->direction;
        this->u = UnitVector(Cross(this->vup, this->w));
        this->v = Cross(this->w, this->u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const auto viewport_u =  this->u * viewport_width;
        const auto viewport_v = -this->v * viewport_height;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        this->pixel_delta_u = viewport_u / (double)this->image_width;
        this->pixel_delta_v = viewport_v / (double)this->image_height;

        // Calculate the location of pixel (0, 0).
        const auto viewport_upper_left = this->origin - this->w * this->focus_distance - viewport_u / 2 - viewport_v / 2;
        this->pixel00_location = viewport_upper_left + 0.5 * (this->pixel_delta_u + this->pixel_delta_v);

        const auto defocus_radius = this->focus_distance * std::tan(DegreesToRadians(this->defocus_angle / 2));
        this->defocus_disk_u = u * defocus_radius;
        this->defocus_disk_v = v * defocus_radius;
    }

    Ray GetRay(const size_t x, const size_t y) const
    {
        // Construct a camera ray origintating from the defocused disk and directed
        // at randomly sampled point around the pixel location (x, y).

        const auto offset = SampleSquare();
        const auto pixel_sample =
            this->pixel00_location +
            ((x + offset.x()) * this->pixel_delta_u) +
            ((y + offset.y()) * this->pixel_delta_v);
        
        const auto ray_origin = (defocus_angle <= 0) ? this->origin : DefocusDiskSample();
        const auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    Vec3 SampleSquare() const
    {
        // Returns the vector to a random point in the [-.5,-5]-[+.5,+.5] unit square.
        return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0);
    }

    Point3 DefocusDiskSample() const
    {
        const auto p = RandomInUnitDisk();
        return this->origin + (p.x() * this->defocus_disk_u) + (p.y() * this->defocus_disk_v);
    }

    Color RayColor(const Ray& ray, const size_t depth, const Hittable& world) const
    {
        if (depth < 1)
        {
            return Color(0, 0, 0);
        }

        HitRecord hit_record;

        if (world.Hit(ray, Interval(0.001, K_INFINITY), hit_record))
        {
            Ray scattered;
            Color attenuation;
            if (hit_record.material->Scatter(ray, hit_record, attenuation, scattered))
            {
                return attenuation * RayColor(scattered, depth - 1, world);
            }

            return Color(0, 0, 0);
        }

        // Background. 
        const auto a = 0.5 * (UnitVector(ray.Direction()).y() + 1.0);
        return (1 - a) * Color(1, 1, 1) + a * Color(0.5, 0.7, 1.0);
    }
};