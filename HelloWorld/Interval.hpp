#pragma once

#include <limits>

class Interval
{
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {}

    Interval(const double min, const double max) : min(min), max(max) {}

    Interval(const Interval& a, const Interval& b)
    {
        this->min = std::min(a.min, b.min);
        this->max = std::max(a.max, b.max);
    }

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

    Interval Expand(const double delta) const
    {
        const auto padding = delta / 2;
        return Interval(this->min - padding, this->max + padding);
    }

    static const Interval Empty, Universe;
};

const Interval Interval::Empty    = Interval(+infinity, -infinity);
const Interval Interval::Universe = Interval(-infinity, +infinity);
