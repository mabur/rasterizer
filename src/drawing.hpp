#pragma once

#include "mesh.hpp"
#include "vector_space.hpp"
#include "sdl_wrappers.hpp"

void drawPoint(Sdl& sdl, const Vector4d& vertex_image);

void drawPoints(Sdl& sdl, const Vectors4d& vertices_image);

void drawTriangles(
    Sdl& sdl, const Vectors4d& vertices_image, const Triangles& triangles);
