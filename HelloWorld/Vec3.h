#pragma once

#include <iostream>

class Vec3
{
public:
    double e[3];

    Vec3() : e{ 0, 0, 0 } {}
    Vec3(const double e0, const double e1, const double e2) : e{ e0, e1, e2 } {}

    double x() const { return this->e[0]; }
    double y() const { return this->e[1]; }
    double z() const { return this->e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    Vec3& operator+=(const Vec3& v)
    {
        e[0] += v[0];
        e[1] += v[1];
        e[2] += v[2];
        return *this;
    }

    Vec3& operator-=(const Vec3& v)
    {
        
        return *this += -v;
    }

    Vec3& operator*=(const double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3& operator/=(double t)
    {
        return *this *= 1 / t;
    }

    double LengthSquared() const
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double Length() const
    {
        return std::sqrt(LengthSquared());
    }

    bool NearZero() const
    {
        constexpr auto s = 1e-8;
        return (std::abs(e[0]) < s) && (std::abs(e[1]) < s) && (std::abs(e[2]) < s);
    }
};

using Point3 = Vec3;

inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
{
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline Vec3 operator+(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline Vec3 operator-(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline Vec3 operator*(const Vec3& u, const Vec3& v)
{
    return Vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline Vec3 operator*(const double t, const Vec3& v)
{
    return Vec3(t * v.x(), t * v.y(), t * v.z());
}

inline Vec3 operator*(const Vec3& v, const double t)
{
    return t * v;
}

inline Vec3 operator/(const Vec3& v, const double t)
{
    return (1 / t) * v;
}

inline double Dot(const Vec3& u, const Vec3& v)
{
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

inline Vec3 Cross(const Vec3& u, const Vec3& v)
{
    return Vec3(
        u.y() * v.z() - u.z() * v.y(),
        u.z() * v.x() - u.x() * v.z(),
        u.x() * v.y() - u.y() * v.x()
    );
}

inline Vec3 UnitVector(const Vec3& v)
{
    return v / v.Length();
}

Vec3 Random()
{
    return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
}

Vec3 Random(const double min, const double max)
{
    return Vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
}

inline Vec3 RandomUnitVector()
{
    while (true)
    {
        auto p = Random(-1, 1);
        const auto lensq = p.LengthSquared();
        if (lensq > 1e-160 && lensq <= 1)
        {
            return p / std::sqrt(lensq);
        }
    }
}

inline Vec3 RandomOnHemisphere(const Vec3& normal)
{
    const Vec3 on_unit_sphere = RandomUnitVector();

    // In the same hemisphere as normal.
    if (Dot(on_unit_sphere, normal) > 0.0)
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}

inline Vec3 RandomInUnitDisk()
{
    while (true)
    {
        const auto p = Vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
        if (p.LengthSquared() < 1)
        {
            return p;
        }
    }
}

inline Vec3 Reflect(const Vec3& v, const Vec3& n)
{
    return v - 2 * Dot(v, n) * n;
}

inline Vec3 Refract(const Vec3& uv, const Vec3& n, const double etai_over_etat)
{
    // Why use std::min()?
    const auto cos_theta = std::min(Dot(-uv, n), 1.0);

    // NOTE: Given ray R it is possible to decompose it to R = R_perp + R_prll.
    // However, I have no idea what the code below is. I can understand how to
    // construct R_perp and R_prll to be, respectively, perpendicular and
    // parallel to X and Y axes, but this code constructs them parallel to the
    // surface normal. I would really love to know how it came up to be this.
    // NOTE: It involves refraction indices' ratio (etai_over_etat). Keep that
    // in mind.

    const Vec3 ray_out_perpendicular = etai_over_etat * (uv + cos_theta * n);
    const Vec3 ray_out_parallel = -std::sqrt(std::abs(1.0 - ray_out_perpendicular.LengthSquared())) * n;

    return ray_out_perpendicular + ray_out_parallel;
}
