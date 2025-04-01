#pragma once

#include "RTWeekend.hpp"
#include "Vec3.hpp"

#include <cmath>

class Perlin
{
public:
    Perlin()
    {
        for (int i = 0; i < point_count; ++i)
        {
            random_doubles[i] = RandomDouble();
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

        double u = p.x() - std::floor(p.x());
        double v = p.y() - std::floor(p.y());
        double w = p.z() - std::floor(p.z());
        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        const int i = int(std::floor(p.x()));
        const int j = int(std::floor(p.y()));
        const int k = int(std::floor(p.z()));
        double c[2][2][2];

        for (int di = 0; di < 2; ++di)
        {
            for (int dj = 0; dj < 2; ++dj)
            {
                for (int dk = 0; dk < 2; ++dk)
                {
                    c[di][dj][dk] = random_doubles[
                        perm_x[(i + di) & 0b11111111] ^
                        perm_y[(j + dj) & 0b11111111] ^
                        perm_z[(k + dk) & 0b11111111]
                    ];
                }
            }
        }

        return TrilinearInterpolation(c, u, v, w);
    }

private:
    static const int point_count = 256;
    double random_doubles[point_count];
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

    static double TrilinearInterpolation(const double c[2][2][2], const double u, const double v, const double w)
    {
        double result = 0;
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    result += c[i][j][k] *
                        (i * u + (1 - i) * (1 - u)) *
                        (j * v + (1 - j) * (1 - v)) *
                        (k * w + (1 - k) * (1 - w));
                }
            }
        }
        return result;
    }
};
