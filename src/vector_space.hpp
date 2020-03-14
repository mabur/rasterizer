#pragma once
#include <Eigen/Core>
#include <Eigen/StdVector>

using Vector2d = Eigen::Vector2d;
using Vector4d = Eigen::Vector4d;
using Matrix4d = Eigen::Matrix4d;
using Vectors2d = std::vector<Vector2d, Eigen::aligned_allocator<Vector2d>>;
using Vectors4d = std::vector<Vector4d, Eigen::aligned_allocator<Vector4d>>;
