#pragma once

#include <vector>

#include "vector_space.hpp"

struct Triangles
{
    std::vector<size_t> indices0;
    std::vector<size_t> indices1;
    std::vector<size_t> indices2;
};

Vectors4d makeSphere(int num_points);

void loadModel(const char* filepath, Vectors4d& positions, Triangles& triangles);

void projectPoints(
    const Matrix4d& image_from_world,
    const Vectors4d& vertices_world,
    Vectors4d& vertices_image);