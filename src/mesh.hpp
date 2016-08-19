#pragma once

#include <vector>

#include "vector_space.hpp"

struct Triangles
{
    std::vector<size_t> indices0;
    std::vector<size_t> indices1;
    std::vector<size_t> indices2;
	size_t size() const { return indices0.size(); }
};

Vectors4d makeSphere(int num_points);

void loadModel(const char* filepath, Vectors4d& positions, Triangles& triangles);
