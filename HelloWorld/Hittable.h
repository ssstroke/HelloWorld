#pragma once

#include "Interval.h"
#include "Ray.h"

class Material;

class HitRecord
{
public:
    Point3 point;
    Vec3 normal;
    std::shared_ptr<Material> material;
    double t;
    bool front_face;

    // Sets the hit record normal vector.
    // NOTE: `outward_normal` is assumed to have unit length.
    void SetFaceNormal(const Ray& ray, const Vec3& outward_normal)
    {
        front_face = Dot(ray.Direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, const Interval ray_t, HitRecord& record) const = 0;
};
