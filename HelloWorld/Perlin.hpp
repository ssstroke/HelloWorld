#pragma once

#include "RTWeekend.hpp"
#include "Vec3.hpp"

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
        const auto x = int(4 * p.x()) & 255;
        const auto y = int(4 * p.y()) & 255;
        const auto z = int(4 * p.z()) & 255;
        return random_doubles[perm_x[x] ^ perm_y[y] ^ perm_z[z]];
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
};
