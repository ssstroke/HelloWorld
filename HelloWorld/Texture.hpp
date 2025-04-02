#pragma once

#include "Color.hpp"
#include "Image.hpp"
#include "Interval.hpp"
#include "Perlin.hpp"
#include "Vec3.hpp"

#include <cmath>
#include <memory>
#include <string>

using std::make_shared;
using std::shared_ptr;

class Texture
{
public:
    virtual ~Texture() = default;

    virtual Color Value(const double u, const double v, const Point3& p) const = 0;
};

class Tex_SolidColor : public Texture
{
public:
    Tex_SolidColor(const Color& albedo) : albedo(albedo) {}

    Tex_SolidColor(const double r, const double g, const double b) : Tex_SolidColor(Color(r, g, b)) {}

    Color Value(const double u, const double v, const Point3& p) const
    {
        return this->albedo;
    }

private:
    Color albedo;
};

class Tex_Checker : public Texture
{
public:
    Tex_Checker(const double scale, const shared_ptr<Texture> even, const shared_ptr<Texture> odd) :
        inv_scale(1.0 / scale), even(even), odd(odd) {}

    Tex_Checker(const double scale, const Color& color_0, const Color& color_1) :
        Tex_Checker(scale, make_shared<Tex_SolidColor>(color_0), make_shared<Tex_SolidColor>(color_1)) {}

    Color Value(const double u, const double v, const Point3& p) const override
    {
        const auto floor_x = int(std::floor(p.x() * this->inv_scale));
        const auto floor_y = int(std::floor(p.y() * this->inv_scale));
        const auto floor_z = int(std::floor(p.z() * this->inv_scale));

        if ((floor_x + floor_y + floor_z) % 2 == 0)
        {
            return this->even->Value(u, v, p);
        }
        else
        {
            return this->odd->Value(u, v, p);
        }
    }

private:
    double inv_scale;

    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

class Tex_Perlin : public Texture
{
public:
    Tex_Perlin(const double scale) : scale(scale) {}

    Color Value(const double u, const double v, const Point3& p) const override
    {
        return Color(0.5, 0.5, 0.5) * (1.0 + perlin.Noise(p * scale));
    }

private:
    Perlin perlin;
    double scale;
};

class Tex_Image : public Texture
{
public:
    Tex_Image(const std::string& filename) : image(filename) {}

    Color Value(double u, double v, const Point3& p) const override
    {
        // Return cyan if texture is missing.
        if (this->image.height <= 0) return Color(0, 1, 1);

        u = Interval(0, 1).Clamp(u);
        v = 1.0 - Interval(0, 1).Clamp(v);

        const auto i = int(u * image.width);
        const auto j = int(v * image.height);
        const auto pixel = image.PixelData(i, j);

        const auto color_scale = 1.0 / 255.0;

        return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    Image image;
};
