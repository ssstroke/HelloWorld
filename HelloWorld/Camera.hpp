// ↑ ↓ → ←
//
// I use right-handed coordinates (still can't understand why are they called like that lol).
// X-axis goes right, Y-axis goes up, negative Z-axis points in the viewing direction.

#pragma once

#include "Color.hpp"
#include "Hittable.hpp"
#include "Image.hpp"
#include "Interval.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include "RTWeekend.hpp"
#include "Vec3.hpp"

#include <cmath>
#include <iostream>
#include <string>

class Camera
{
public:
    std::string image_filename = "output/default.png";

    int image_width  = 100;  // Rendered image width in pixels
    int image_height = 100;  // Rendered image height in pixels

    Point3 origin       = Point3(0, 0, 0);  
    Vec3   direction    = Vec3(0, 0, -1);
    Vec3   direction_up = Vec3(0, 1, 0);    // Camera-relative "up" direction

    double fov_vertical = 90;  // Vertical view angle (field of view)

    double defocus_angle  = 0;   // Variation angle of rays through each pixel
    double focus_distance = 10;  // Distance from camera origin to plane of perfect focus

    int samples_per_pixel = 10;  // Count of random samples for each pixel
    int max_depth         = 10;  // Maximum number of ray bounces into scene

    Color background;  // Scene background color

    Camera() {}

    void Render(const Hittable& world)
    {
        this->Initialize();

        for (int h = 0; h < this->image_height; ++h)
        {
            std::clog << "Scanline " << h << " out of " << this->image_height - 1 << ".\n";
            for (int w = 0; w < this->image_width; ++w)
            {
                Color pixel_color(0, 0, 0);

                for (size_t sample = 0; sample < this->samples_per_pixel; ++sample)
                {
                    const Ray ray = this->GetRay(w, h);
                    pixel_color += RayColor(ray, this->max_depth, world);
                }

                image.WriteColor(w, h, this->pixel_samples_scale * pixel_color);
            }
        }

        if (image.WritePNG(this->image_filename))
        {
            std::clog << "Write to " << this->image_filename << ".\n";
        }
        else
        {
            std::clog << "Failed to write to " << this->image_filename << ".\n";
        }
    }

private:
    Image image = Image(this->image_width, this->image_height);

    double aspect_ratio = 1.0;  // Ratio of image width over height

    Point3 pixel00_location;  // Location of pixel (0, 0)
    Vec3   pixel_delta_u;     // Offset to pixel to the right
    Vec3   pixel_delta_v;     // Offset to pixel to the bottom

    double pixel_samples_scale = 1;  // Color scale factor for a sum of pixel samples

    Vec3 u, v, w;                        // Camera frame basis vectors (right, up, opposite view direction)

    Vec3 defocus_disk_u, defocus_disk_v; // Defocus disk horizontal, vertical radius

    void Initialize()
    {
        this->image = Image(this->image_width, this->image_height);

        this->pixel_samples_scale = 1.0 / samples_per_pixel;

        this->aspect_ratio = (double)this->image_width / (double)this->image_height;

        // Viewport dimensions.
        const double theta = DegreesToRadians(this->fov_vertical);
        const double h = std::tan(theta / 2);
        const double viewport_height = 2 * h * this->focus_distance;
        const double viewport_width = viewport_height * this->aspect_ratio;

        // Calculate u, v, w.
        this->w = -this->direction;
        this->u = UnitVector(Cross(this->direction_up, this->w));
        this->v = Cross(this->w, this->u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const Vec3 viewport_u = this->u * viewport_width;
        const Vec3 viewport_v = -this->v * viewport_height;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        this->pixel_delta_u = viewport_u / (double)this->image_width;
        this->pixel_delta_v = viewport_v / (double)this->image_height;

        // Calculate the location of pixel (0, 0).
        const Vec3 viewport_upper_left = this->origin - this->w * this->focus_distance - viewport_u / 2 - viewport_v / 2;
        this->pixel00_location = viewport_upper_left + 0.5 * (this->pixel_delta_u + this->pixel_delta_v);

        const double defocus_radius = this->focus_distance * std::tan(DegreesToRadians(this->defocus_angle / 2));
        this->defocus_disk_u = u * defocus_radius;
        this->defocus_disk_v = v * defocus_radius;
    }

    Ray GetRay(const int x, const int y) const
    {
        // Construct a camera ray origintating from the defocused disk and directed
        // at randomly sampled point around the pixel location (x, y).

        const Vec3 offset = SampleSquare();
        const Vec3 pixel_sample =
            this->pixel00_location +
            ((x + offset.x()) * this->pixel_delta_u) +
            ((y + offset.y()) * this->pixel_delta_v);

        const Vec3 ray_origin = (defocus_angle <= 0) ? this->origin : DefocusDiskSample();
        const Vec3 ray_direction = pixel_sample - ray_origin;
        const double ray_time = RandomDouble();

        return Ray(ray_origin, ray_direction, ray_time);
    }

    Vec3 SampleSquare() const
    {
        // Returns the vector to a random point in the [-.5,-5]-[+.5,+.5] unit square.
        return Vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0);
    }

    Point3 DefocusDiskSample() const
    {
        const Vec3 p = RandomInUnitDisk();
        return this->origin + (p.x() * this->defocus_disk_u) + (p.y() * this->defocus_disk_v);
    }

    Color RayColor(const Ray& ray, const int depth, const Hittable& world) const
    {
        if (depth < 1)
        {
            return Color(0, 0, 0);
        }

        HitRecord hit_record;

        if (world.Hit(ray, Interval(0.001, infinity), hit_record) == false)
        {
            return this->background;
        }
        else
        {
            Ray scattered;
            Color attenuation;
            const Color color_emitted = hit_record.material->Emit(hit_record.u, hit_record.v, hit_record.point);

            if (hit_record.material->Scatter(ray, hit_record, attenuation, scattered) == false)
            {
                return color_emitted;
            }
            
            const Color color_scattered = attenuation * this->RayColor(scattered, depth - 1, world);

            return color_emitted + color_scattered;
        }

        // Background. 
        const double a = 0.5 * (UnitVector(ray.Direction()).y() + 1.0);
        return (1 - a) * Color(1, 1, 1) + a * Color(0.5, 0.7, 1.0);
    }
};
