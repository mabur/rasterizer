#pragma once

#include <string>
#include <vector>

#include "texture.hpp"
#include "vector_space.hpp"

struct Triangles
{
    std::vector<size_t> indices0;
    std::vector<size_t> indices1;
    std::vector<size_t> indices2;
    std::vector<size_t> texture_indices;
	size_t size() const { return indices0.size(); }
};

Vectors4d makeSphere(int num_points);

void loadModel(
    const std::string& filepath,
    Vectors4d& positions_world,
    Vectors2d& positions_texture,
    Triangles& triangles,
    Textures& textures);
