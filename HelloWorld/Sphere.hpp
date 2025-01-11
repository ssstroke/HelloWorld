#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable
{
public:
    Sphere(const Point3& center, const double radius, const std::shared_ptr<Material> material) : center(center), radius(std::max(0.0, radius)), material(material)
    {
        // TODO: Initialize the material pointer.
    }

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& record) const override
    {
        const Vec3 OC = center - ray.Origin();
        const auto A = ray.Direction().LengthSquared();
        const auto H = Dot(ray.Direction(), OC);
        const auto C = OC.LengthSquared() - radius * radius;

        const auto DISCRIMINANT = H * H - A * C;
        if (DISCRIMINANT < 0)
        {
            return false;
        }

        const auto SQRT_DISCRIMINANT = std::sqrt(DISCRIMINANT);

        // Find the nearest root that lies in the acceptable range.
        // Why do I even need this range?
        // I guess this is to disallow negative ray direction.
        auto root = (H - SQRT_DISCRIMINANT) / A;
        if (ray_t.Surrounds(root) == false)
        {
            root = (H + SQRT_DISCRIMINANT) / A;
            if (ray_t.Surrounds(root) == false)
            {
                return false;
            }
        }

        record.t = root;
        record.point = ray.At(record.t);
        const auto OUTWARD_NORMAL = (record.point - center) / radius;
        record.SetFaceNormal(ray, OUTWARD_NORMAL);
        record.material = this->material;

        return true;
    }

private:
    Point3 center;
    double radius;
    std::shared_ptr<Material> material;
};

