#pragma once

#include "Color.hpp"
#include "Interval.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#pragma warning(disable : 4996) // `std_image_write.h` uses `sprintf`.
#define STB_IMAGE_WRITE_IMPLEMENTATION // must be defined before #including
#include <stb_image_write.h>

class Image
{
public:
    int width = 0;
    int height = 0;

    // Create an image with specified <width>, <height> parameters.
    Image(const int width, const int height) : width(width), height(height), data(width * height * 3) {}

    // Load an image by specified <filename>.
    Image(const std::string& filename)
    {
        auto n = this->bytes_per_pixel;
        this->fdata = stbi_loadf(filename.c_str(), &this->width, &this->height, &n, this->bytes_per_pixel);
        if (this->fdata == nullptr)
        {
            std::cerr << "[ERROR]:\tCould not load image `" << filename << "'\n";
        }

        this->bytes_per_scanline = this->width * this->bytes_per_pixel;
        ConvertToBytes();
    }

    ~Image()
    {
        if (this->fdata != nullptr)
        {
            stbi_image_free(this->fdata);
        }
    }

    const uint8_t* PixelData(const int x, const int y) const
    {
        static uint8_t magenta[] = { 255, 0, 255 };
        if (data.size() == 0)
        {
            return magenta;
        }

        return &data.data()[Clamp(y, 0, width) * bytes_per_scanline + Clamp(x, 0, height) * bytes_per_pixel];
    }

    void WriteColor(const int x, const int y, const Color& color)
    {
        const auto r = LinearToGamma(color.x());
        const auto g = LinearToGamma(color.y());
        const auto b = LinearToGamma(color.z());

        static const Interval intensity(0.000, 0.999);

        // I fucking hate this.
        //

        const int    offset_int    = (x + y * this->width) * 3;
        const size_t offset_size_t = size_t(offset_int);

        this->data[offset_size_t + 0] = uint8_t(intensity.Clamp(r) * 255.999);
        this->data[offset_size_t + 1] = uint8_t(intensity.Clamp(g) * 255.999);
        this->data[offset_size_t + 2] = uint8_t(intensity.Clamp(b) * 255.999);
    }

    int WritePNG(const std::string filename)
    {
        return stbi_write_png(
            filename.data(),
            this->width,
            this->height,
            3,
            this->data.data(),
            this->width * 3
        );
    }

private:
    int bytes_per_pixel    = 3; // TODO: Make `const`
    int       bytes_per_scanline = 0;

    float*               fdata = nullptr;
    std::vector<uint8_t> data;

    inline static int Clamp(const int x, const int low, const int high)
    {
        if (x < low) return low;
        if (x < high) return x;
        return high - 1;
    }

    inline static uint8_t FloatToByte(const float value)
    {
        if (value <= 0.0f)
        {
            return 0;
        }
        if (1.0f <= value)
        {
            return 255;
        }
        return uint8_t(256.0f * value);
    }

    void ConvertToBytes()
    {
        const size_t total_bytes = size_t(this->width) * this->height * this->bytes_per_pixel;
        this->data.resize(total_bytes);

        float* fdata_ptr = fdata;

        for (size_t i = 0; i < total_bytes; ++i, ++fdata_ptr)
        {
            this->data[i] = FloatToByte(*fdata_ptr);
        }
    }

    inline static double LinearToGamma(const double linear_component)
    {
        if (linear_component > 0)
        {
            return std::sqrt(linear_component);
        }
        return 0;
    }
};
