#pragma once

#include <limits>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi       = 3.1415926535897932385;

// Utility Functions

inline double DegreesToRadians(const double degrees)
{
    return degrees * pi / 180;
}

inline double RandomDouble(const double min = 0, const double max = 1)
{
    // https://github.com/RayTracing/raytracing.github.io/discussions/1680
    std::uniform_real_distribution<double> distribution(min, max);

    static std::mt19937 generator;

    return distribution(generator);
}
