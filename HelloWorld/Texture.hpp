#pragma once

#include "Color.hpp"
#include "Vec3.hpp"

#include <cmath>
#include <memory>

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
