#pragma once

#include "Vec3.hpp"

class Ray
{
public:
    Ray() {}
    Ray(const Point3& origin, const Vec3& direction) : origin(origin), direction(direction), time(0) {}
    Ray(const Point3& origin, const Vec3& direction, const double time) : origin(origin), direction(direction), time(time) {}

    const Point3& Origin() const
    {
        return this->origin;
    }
    const Vec3& Direction() const
    { 
        return this->direction; 
    }

    Point3 At(const double t) const
    {
        return this->origin + t * this->direction;
    }

    double Time() const
    {
        return time;
    }

private:
    Point3 origin;
    Vec3 direction;
    double time;
};
