#pragma once

#include <string>
#include <valarray>
#include <vector>
#include "vector_space.hpp"

enum { RED, GREEN, BLUE, DUMMY };

class Texture
{
public:
    Texture() : width_(0), height_(0) {}
    Texture(size_t width, size_t height)
        : width_(width)
        , height_(height)
        , colors_(width * height)
    {}
    bool  empty() const { return size() == 0; }
    size_t size() const { return width_ * height_; }
    Vector4d&        operator[](size_t i)       { return colors_[i]; }
    const Vector4d&  operator[](size_t i) const { return colors_[i]; }

    const Vector4d& sample(double x, double y) const
    {
        while (x < 0.0) x += 1.0;
        while (y < 0.0) y += 1.0;
        while (1.0 < x) x -= 1.0;
        while (1.0 < y) y -= 1.0;

        const auto xi = static_cast<size_t>(x * width_);
        const auto yi = static_cast<size_t>(y * height_);
        const auto i = yi * width_ + xi;

        return colors_[i];
    }
private:
    size_t width_;
    size_t height_;
    Vectors4d colors_;
};


using Textures = std::vector<Texture>;

Texture readTexture(const std::string& filepath);


