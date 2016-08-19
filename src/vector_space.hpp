#pragma once
#include <Eigen/Core>
#include <Eigen/StdVector>

using Vector4d = Eigen::Vector4d;
using Matrix4d = Eigen::Matrix4d;
using Vectors4d = std::vector<Vector4d, Eigen::aligned_allocator<Vector4d>>;

template<typename T>
T clamp(T x, T low, T high)
{
    return std::min(std::max(x, low), high);
}