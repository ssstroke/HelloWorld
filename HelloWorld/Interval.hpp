#pragma once

#include <limits>

class Interval
{
public:
    double min, max;

    Interval() : min(+std::numeric_limits<double>().infinity()), max(-std::numeric_limits<double>().infinity()) {}

    Interval(const double min, const double max) : min(min), max(max) {}

    double Size() const
    {
        return this->max - this->min;
    }

    bool Contains(const double x) const
    {
        return this->min <= x && x <= this->max;
    }

    bool Surrounds(const double x) const
    {
        return this->min < x && x < this->max;
    }

    double Clamp(const double x) const
    {
        if (x < this->min) return min;
        if (x > this->max) return max;
        return x;
    }

    static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+std::numeric_limits<double>().infinity(), -std::numeric_limits<double>().infinity());
const Interval Interval::universe = Interval(-std::numeric_limits<double>().infinity(), +std::numeric_limits<double>().infinity());
