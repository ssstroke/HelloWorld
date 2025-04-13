#pragma once

#include "Interval.hpp"
#include "Ray.hpp"
#include "Vec3.hpp"

#include <utility>

class AABB
{
public:
    Interval x, y, z;

    AABB() {}

    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z)
    {
        PadToMinimus();
    }

    AABB(const Point3& a, const Point3& b)
    {
        this->x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
        this->y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
        this->z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

        PadToMinimus();
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
        const Point3& ray_origin = ray.Origin();
        const Vec3& ray_direction = ray.Direction();

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

    int LongestAxis() const
    {
        if (this->x.Size() > this->y.Size())
        {
            if (this->x.Size() > this->z.Size())
            {
                return 0;
            }
            else
            {
                return 2;
            }
        }
        else
        {
            if (this->y.Size() > this->z.Size())
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
    }

    static const AABB Empty, Universe;

private:
    void PadToMinimus()
    {
        const double delta = 0.0001;
        if (x.Size() < delta) x = x.Expand(delta);
        if (y.Size() < delta) y = y.Expand(delta);
        if (z.Size() < delta) z = z.Expand(delta);
    }
};

const AABB AABB::Empty = AABB(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe = AABB(Interval::Universe, Interval::Universe, Interval::Universe);

AABB operator+(const AABB& bbox, const Vec3& offset)
{
    return AABB(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

AABB operator+(const Vec3& offset, const AABB& bbox)
{
    return bbox + offset;
}
