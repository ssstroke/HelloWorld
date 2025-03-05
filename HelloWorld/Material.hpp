#pragma once

#include "Color.hpp"
#include "Hittable.hpp"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const
    {
        return false;
    }
};

class Lambertian : public Material
{
public:
    Lambertian(const Color& albedo) : albedo(albedo) {}

    bool Scatter(const Ray& ray_in, const HitRecord& hit_record, Color& attenuation, Ray& scattered) const override
    {
        auto scatter_direction = hit_record.normal + RandomUnitVector();
        if (scatter_direction.NearZero())
        {
            scatter_direction = hit_record.normal;
        }

        scattered = Ray(hit_record.point, scatter_direction, ray_in.Time());
        attenuation = albedo;

        return true;
    }

private:
    Color albedo;
};

class Metal : public Material
{
public:
    Metal(const Color& albedo, const double fuzz) : albedo(albedo), fuzz(fuzz) {}

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
    double fuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(const double refraction_index) : refraction_index(refraction_index) {}

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
        const auto r0 = std::pow((1 - refraction_index) / (1 + refraction_index), 2);
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};
