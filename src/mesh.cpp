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

std::string getDirectoryPath(const std::string filepath)
{
    return filepath.substr(0, filepath.find_last_of("/\\") + 1);
}

std::string stripFileExtension(const std::string filepath)
{
    return filepath.substr(0, filepath.find_last_of("."));
}

void loadModel(
    const std::string& filepath,
    Vectors4d& positions_world,
    Vectors2d& positions_texture,
    Triangles& triangles,
    Textures& textures)
{
    using namespace std;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;
    string error_message;

    cout << "Loading model..." << endl;

    const auto dirpath = getDirectoryPath(filepath);

    if (!tinyobj::LoadObj(shapes, materials, error_message, filepath.c_str(), dirpath.c_str()))
        cerr << error_message << endl;

    cout << "# of shapes    : " << shapes.size() << endl;
    cout << "# of materials : " << materials.size() << endl;

    const auto& mesh = shapes.front().mesh;
    assert(mesh.positions.size() % 3 == 0);
    assert(mesh.texcoords.size() % 2 == 0);
    assert(mesh.positions.size() / 3 == mesh.texcoords.size() / 2);

    const auto num_positions = mesh.positions.size() / 3;

    positions_world   = Vectors4d(num_positions);
    positions_texture = Vectors2d(num_positions);

    for (size_t i = 0; i < num_positions; ++i)
    {
        auto x = double{ mesh.positions[3 * i + 0] };
        auto y = double{ mesh.positions[3 * i + 1] };
        auto z = double{ mesh.positions[3 * i + 2] };
        auto w = 1.0;
        positions_world[i] = Vector4d{ x, y, z, w };
    }

    for (size_t i = 0; i < num_positions; ++i)
    {
        auto x = double{ mesh.texcoords[2 * i + 0] };
        auto y = double{ mesh.texcoords[2 * i + 1] };
        positions_texture[i] = Vector2d{ x, y };
    }

    const auto num_triangles = mesh.indices.size() / 3;

    for (size_t f = 0; f < num_triangles; ++f)
    {
        triangles.indices0.push_back(mesh.indices[3 * f + 0]);
        triangles.indices1.push_back(mesh.indices[3 * f + 1]);
        triangles.indices2.push_back(mesh.indices[3 * f + 2]);
        triangles.texture_indices.push_back(mesh.material_ids[f]);
    }

    textures = Textures(materials.size());
    for (size_t i = 0; i < textures.size(); ++i)
    {
        const auto filename_png = materials[i].ambient_texname;
        if (filename_png.empty())
            continue;
        const auto filename = stripFileExtension(filename_png);
        const auto filename_ppm = filename + ".ppm";
        const auto filepath_ppm = dirpath + filename_ppm;
        textures[i] = readTexture(filepath_ppm);
    }
}

