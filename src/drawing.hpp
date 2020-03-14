#pragma once

#include "camera.hpp"
#include "mesh.hpp"
#include "vector_space.hpp"
#include "sdl_wrappers.hpp"
#include "texture.hpp"

namespace vertex_index {enum {BARY0, BARY1, BARY2, DISPARITY, U, V, X, Y, Z, SIZE};}
using Vertex = Eigen::Matrix<double, vertex_index::SIZE, 1>;
using Pixel = Uint32;

struct Light
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Vector4d position_world;
    Vector4d power;
};

struct Environment
{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    CameraIntrinsics intrinsics;
    CameraExtrinsics extrinsics;
    Light light;
};

struct Vertices
{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Vertices(size_t num_vertices)
		: positions_world(num_vertices), positions_image(num_vertices), positions_texture(num_vertices)
	{}
	Vectors4d positions_world;
	Vectors4d positions_image;
    Vectors2d positions_texture;
	size_t size() const { return positions_world.size(); }
};

struct Pixels
{
	Pixels(int width, int height)
		: width(width)
		, height(height)
		, colors(width * height)
		, disparities(width * height)
	{}
	std::vector<Uint32> colors;
	std::vector<double> disparities;
	size_t width;
	size_t height;
	size_t size() const { return width * height; }
};

void vertexShader(Vertices& vertices, const Environment& environment);
void drawPoint(Pixels& pixels, const Vector4d& vertex_image);
void drawPoints(Pixels& pixels, const Vectors4d& vertices_image);
void drawTriangles(Pixels& pixels, const Vertices& vertices,
    const Triangles& triangles, const Textures& textures, const Environment& environment);
bool isBehindCamera(const Vector4d& v0, const Vector4d& v1, const Vector4d& v2);
Light makeLight();
