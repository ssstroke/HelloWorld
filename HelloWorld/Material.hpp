#pragma once

#include "Color.hpp"
#include "Hittable.hpp"
#include "Ray.hpp"
#include "RTWeekend.hpp"
#include "Texture.hpp"
#include "Vec3.hpp"

#include <cmath>
#include <memory>
#include <utility>

using std::make_shared;
using std::shared_ptr;

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const
    {
        return false;
    }

    virtual Color Emit(const double u, const double v, const Point3& p) const
    {
        return Color(0, 0, 0);
    }
};

class Mat_Lambertian : public Material
{
public:
    Mat_Lambertian(const Color& albedo) : texture(make_shared<Tex_SolidColor>(albedo)) {}

    Mat_Lambertian(const shared_ptr<Texture> texture) : texture(texture) {}

    bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const override
    {
        Vec3 scatter_direction = hit_record.normal + RandomUnitVector();
        if (scatter_direction.NearZero())
        {
            scatter_direction = hit_record.normal;
        }

        scattered = Ray(hit_record.point, scatter_direction, ray_in.Time());
        attenuation = texture->Value(hit_record.u, hit_record.v, hit_record.point);

        return true;
    }

private:
    shared_ptr<Texture> texture;
};

class Mat_Metal : public Material
{
public:
    Mat_Metal(const Color& albedo, const double fuzz) : albedo(albedo), fuzz(fuzz) {}

    bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const override
    {
        Vec3 reflected = Reflect(ray_in.Direction(), hit_record.normal);
        reflected = UnitVector(reflected) + (fuzz * RandomUnitVector());

        scattered = Ray(hit_record.point, reflected, ray_in.Time());
        attenuation = albedo;

        return (Dot(scattered.Direction(), hit_record.normal) > 0);
    }

private:
    Color albedo;
    double fuzz = 0;
};

class Mat_Dielectric : public Material
{
public:
    Mat_Dielectric(const double refraction_index) : refraction_index(refraction_index) {}

    bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const override
    {
        // Attenuation is always 1 — the glass surface absorbs nothing.
        attenuation = Color(1, 1, 1);

        const double ri = hit_record.front_face ? (1.0 / this->refraction_index) : this->refraction_index;

        const Vec3 unit_direction = UnitVector(ray_in.Direction());

        const double cos_theta = std::min(Dot(-unit_direction, hit_record.normal), 1.0);
        const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        const bool cant_refract = (ri * sin_theta) > 1.0;

        Vec3 direction;
        if (cant_refract || Reflectance(cos_theta, ri) > RandomDouble())
        {
            direction = Reflect(unit_direction, hit_record.normal);
        }
        else
        {
            direction = Refract(unit_direction, hit_record.normal, ri);
        }

        scattered = Ray(hit_record.point, direction, ray_in.Time());

        return true;
    }

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index
    // over the refractive index of enclosing media.
    double refraction_index;

    static double Reflectance(const double cosine, const double refraction_index)
    {
        // Use Schlik's approximation for reflectance.
        const double r0 = std::pow((1 - refraction_index) / (1 + refraction_index), 2);
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

class Mat_DiffuseLight : public Material
{
public:
    Mat_DiffuseLight(shared_ptr<Texture> tex) : texture(tex) {}
    Mat_DiffuseLight(const Color& emit) : texture(make_shared<Tex_SolidColor>(emit)) {}

    Color Emit(const double u, const double v, const Point3& p) const override
    {
        return texture->Value(u, v, p);
    }

private:
    shared_ptr<Texture> texture;
};
