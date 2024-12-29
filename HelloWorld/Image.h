#pragma once

#include "Color.h"
#include "Interval.h"

#include <cstdint>
#include <string>
#include <vector>

#pragma warning(disable : 4996) // `std_image_write.h` uses `sprintf`.
#define STB_IMAGE_WRITE_IMPLEMENTATION // must be defined before #including
#include <stb_image_write.h>

class Image {
public:
    std::vector<uint8_t> data;
    size_t width;
    size_t height;

    Image(const size_t width, const size_t height) : width(width), height(height), data(width * height * 3) {}

    inline static double LinearToGamma(const double linear_component)
    {
        if (linear_component > 0)
        {
            return std::sqrt(linear_component);
        }

        return 0;
    }

    void WriteColor(const size_t x, const size_t y, const Color& c)
    {
        assert(x < width);
        assert(y < height);

        const auto r = LinearToGamma(c.x());
        const auto g = LinearToGamma(c.y());
        const auto b = LinearToGamma(c.z());

        assert(r >= 0.0 && r <= 1.0);
        assert(g >= 0.0 && g <= 1.0);
        assert(b >= 0.0 && b <= 1.0);

        static const Interval intensity(0.000, 0.999);

        const size_t offset = (x + y * this->width) * 3;

        this->data[offset + 0] = uint8_t(intensity.Clamp(r) * 255.999);
        this->data[offset + 1] = uint8_t(intensity.Clamp(g) * 255.999);
        this->data[offset + 2] = uint8_t(intensity.Clamp(b) * 255.999);
    }

    int WritePNG(const std::string filename)
    {
        return stbi_write_png(
            filename.data(),
            (int)this->width,
            (int)this->height,
            3,
            this->data.data(),
            (int)this->width * 3
        );
    }
};
