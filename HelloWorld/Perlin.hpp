#pragma once

#include "RTWeekend.hpp"

#include "Vec3.hpp"

#include <cmath>
#include <cstdlib>

class Perlin
{
public:
    Perlin()
    {
        for (int i = 0; i < point_count; ++i)
        {
            random_vectors[i] = UnitVector(Vec3::Random(-1, 1));
        }
        GeneratePerm(perm_x);
        GeneratePerm(perm_y);
        GeneratePerm(perm_z);
    }

    double Noise(const Point3& p) const
    {
        // Description: construct a 1x1 cube with points having
        //  values of Perlin(floor(p.x()), Perlin(floor(p.x() + 1)
        //  etc. for each component. Then map Point(p) to space
        //  inside this 1x1 cube and interpolate.

        const double u = p.x() - std::floor(p.x());
        const double v = p.y() - std::floor(p.y());
        const double w = p.z() - std::floor(p.z());

        const int i = int(std::floor(p.x()));
        const int j = int(std::floor(p.y()));
        const int k = int(std::floor(p.z()));
        Vec3 c[2][2][2];

        for (int di = 0; di < 2; ++di)
        {
            for (int dj = 0; dj < 2; ++dj)
            {
                for (int dk = 0; dk < 2; ++dk)
                {
                    c[di][dj][dk] = random_vectors[
                        perm_x[(i + di) & 0b11111111] ^
                        perm_y[(j + dj) & 0b11111111] ^
                        perm_z[(k + dk) & 0b11111111]
                    ];
                }
            }
        }

        return PerlinInterpolation(c, u, v, w);
    }

    double Turbulence(const Point3& p, const int depth) const
    {
        double result = 0;
        Point3 p_temp = p;
        double weight = 1;

        for (int i = 0; i < depth; ++i)
        {
            result += weight * Noise(p_temp);
            weight *= 0.5;
            p_temp *= 2;
        }

        return std::abs(result);
    }

private:
    static const int point_count = 256;
    Vec3 random_vectors[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void GeneratePerm(int* p)
    {
        for (int i = 0; i < point_count; ++i)
        {
            p[i] = i;
        }
        Permute(p);
    }

    static void Permute(int* p)
    {
        for (int i = point_count - 1; i > 0; --i)
        {
            const int target = RandomInt(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double PerlinInterpolation(const Vec3 c[2][2][2], const double u, const double v, const double w)
    {
        const double uu = u * u * (3 - 2 * u);
        const double vv = v * v * (3 - 2 * v);
        const double ww = w * w * (3 - 2 * w);

        double result = 0;

        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    const Vec3 weight_v(u - i, v - j, w - k);
                    result += Dot(weight_v, c[i][j][k]) *
                        (i * uu + (1 - i) * (1 - uu)) *
                        (j * vv + (1 - j) * (1 - vv)) *
                        (k * ww + (1 - k) * (1 - ww));
                }
            }
        }

        return result;
    }
};
