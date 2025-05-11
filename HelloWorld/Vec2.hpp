#include "RTWeekend.hpp"

#include <cmath>
#include <cstdlib>
#include <iosfwd>
#include <iostream>

class Vec2
{
public:
    double e[2];

    Vec2() : e{ 0, 0 } {}
    Vec2(const double e0, const double e1) : e{ e0, e1 } {}

    double x() const { return this->e[0]; }
    double y() const { return this->e[1]; }

    Vec2 operator-() const { return Vec2(-e[0], -e[1]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vec2& operator+=(const Vec2& v)
    {
        e[0] += v[0];
        e[1] += v[1];
        return *this;
    }

    Vec2& operator-=(const Vec2& v)
    {
        return *this += -v;
    }

    Vec2& operator*=(const double t)
    {
        e[0] *= t;
        e[1] *= t;
        return *this;
    }

    Vec2& operator/=(double t)
    {
        return *this *= 1 / t;
    }

    double LengthSquared() const
    {
        return e[0] * e[0] + e[1] * e[1];
    }

    double Length() const
    {
        return std::sqrt(LengthSquared());
    }

    bool NearZero() const
    {
        constexpr double s = 1e-8;
        return (std::abs(e[0]) < s) && (std::abs(e[1]) < s);
    }

    static Vec2 Random()
    {
        return Vec2(RandomDouble(), RandomDouble());
    }

    static Vec2 Random(const double min, const double max)
    {
        return Vec2(RandomDouble(min, max), RandomDouble(min, max));
    }
};

using Point2 = Vec2;

// --- Utility functions and operators ---

inline std::ostream& operator<<(std::ostream& out, const Vec2& v)
{
    return out << v.x() << ' ' << v.y();
}

inline Vec2 operator+(const Vec2& u, const Vec2& v)
{
    return Vec2(u.x() + v.x(), u.y() + v.y());
}

inline Vec2 operator-(const Vec2& u, const Vec2& v)
{
    return Vec2(u.x() - v.x(), u.y() - v.y());
}

inline Vec2 operator*(const Vec2& u, const Vec2& v)
{
    return Vec2(u.x() * v.x(), u.y() * v.y());
}

inline Vec2 operator*(const double t, const Vec2& v)
{
    return Vec2(t * v.x(), t * v.y());
}

inline Vec2 operator*(const Vec2& v, const double t)
{
    return t * v;
}

inline Vec2 operator/(const Vec2& v, const double t)
{
    return (1 / t) * v;
}
