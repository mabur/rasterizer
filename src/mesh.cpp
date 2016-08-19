#include "mesh.hpp"

#include <cmath>
#include <iostream>
#include <random>

#include "tiny_obj_loader.h"

Vectors4d makeSphere(int num_points)
{
    auto generator = std::default_random_engine();
    auto distribution = std::normal_distribution<double>(0.0, 1.0);
    auto vertices_world = Vectors4d(num_points);

    for (size_t i = 0; i < num_points; ++i)
    {
        auto x = distribution(generator);
        auto y = distribution(generator);
        auto z = distribution(generator);
        auto n = 1.0 / sqrt(x * x + y * y + z * z);

        x *= n;
        y *= n;
        z *= n;

        vertices_world[i] = Vector4d(x, y, z, 1.0);
    }

    return vertices_world;
}

void loadModel(const char* filepath, Vectors4d& positions, Triangles& triangles)
{
    using namespace std;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string error_message;

    cout << "Loading model..." << endl;

    if (!tinyobj::LoadObj(shapes, materials, error_message, filepath))
        cerr << error_message << endl;

    cout << "# of shapes    : " << shapes.size() << endl;
    cout << "# of materials : " << materials.size() << endl;

    const auto& mesh = shapes.front().mesh;
    assert(mesh.positions.size() % 3 == 0);

    const auto num_positions = mesh.positions.size() / 3;

    positions = Vectors4d(num_positions);

    for (size_t i = 0; i < num_positions; ++i)
    {
        auto x = double{ mesh.positions[3 * i + 0] };
        auto y = double{ mesh.positions[3 * i + 1] };
        auto z = double{ mesh.positions[3 * i + 2] };
        auto w = 1.0;
        positions[i] = Vector4d{ x, y, z, w };
    }

    const auto num_triangles = mesh.indices.size() / 3;

    for (size_t f = 0; f < num_triangles; ++f)
    {
        triangles.indices0.push_back(mesh.indices[3 * f + 0]);
        triangles.indices1.push_back(mesh.indices[3 * f + 1]);
        triangles.indices2.push_back(mesh.indices[3 * f + 2]);
    }
}

