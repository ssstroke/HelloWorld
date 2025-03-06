#pragma once

#include "Interval.hpp"
#include "Vec3.hpp"
#include "Ray.hpp"

class AABB
{
public:
    Interval x, y, z;

    AABB() {}

    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z) {}

    AABB(const Point3& a, const Point3& b)
    {
        this->x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        this->y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        this->z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
    }

    AABB(const AABB& a, const AABB& b)
    {
        this->x = Interval(a.x, b.x);
        this->y = Interval(a.y, b.y);
        this->z = Interval(a.z, b.z);
    }

    const Interval& AxisInterval(const int n) const
    {
        if (n == 1) return this->y;
        if (n == 2) return this->z;
        return this->x;
    }

    bool Hit(const Ray& ray, Interval ray_t) const
    {
        const Point3& ray_origin    = ray.Origin();
        const Vec3&   ray_direction = ray.Direction();

        for (int axis = 0; axis < 3; ++axis)
        {
            const Interval& ax = AxisInterval(axis);
            const double adinv = 1.0 / ray_direction[axis];

            const double t0 = (ax.min - ray_origin[axis]) * adinv;
            const double t1 = (ax.max - ray_origin[axis]) * adinv;

            if (t0 < t1)
            {
                ray_t.min = std::max(ray_t.min, t0);
                ray_t.max = std::min(ray_t.max, t1);
            }
            else
            {
                ray_t.min = std::max(ray_t.min, t1);
                ray_t.max = std::min(ray_t.max, t0);
            }

            if (ray_t.max <= ray_t.min)
            {
                return false;
            }
        }

        return true;
    }
};