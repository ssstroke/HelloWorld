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

inline double RandomDouble()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double RandomDouble(const double min, const double max)
{
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}
