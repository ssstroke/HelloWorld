#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable
{
public:
    // Stationary sphere.
    Sphere(const Point3& static_center, const double radius, const std::shared_ptr<Material> material) :
        center(static_center, Vec3(0, 0, 0)), radius(std::max(0.0, radius)), material(material) {}

    // Moving sphere.
    Sphere(const Point3& center_0, const Point3& center_1, const double radius, const std::shared_ptr<Material> material) :
        center(center_0, center_1 - center_0), radius(std::max(0.0, radius)), material(material) {}

    bool Hit(const Ray& ray, const Interval ray_t, HitRecord& record) const override
    {
        const Point3 current_center = center.At(ray.Time());
        const Vec3 OC = current_center - ray.Origin();
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

        const auto outward_normal = (record.point - current_center) / radius;
        record.SetFaceNormal(ray, outward_normal);

        record.material = this->material;

        return true;
    }

private:
    Ray center;
    double radius;
    std::shared_ptr<Material> material;
};
