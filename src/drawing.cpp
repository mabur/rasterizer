#include <algorithm>

#include <Eigen/Core>

#include "algorithm.hpp"
#include "drawing.hpp"
#include "drawing_template.hpp"

bool isBehindCamera(const Vector4d& v0, const Vector4d& v1, const Vector4d& v2)
{
	return v0(2) <= 0 || v1(2) <= 0 || v2(2) <= 0;
}

void vertexShader(Vertices& vertices, const Environment& environment)
{
	const auto num_vertices = vertices.size();
	const auto image_from_world = environment.image_from_world;
	for (size_t i = 0; i < num_vertices; ++i)
	{
		const auto position_world = vertices.positions_world[i];
		const auto position_image = Vector4d{image_from_world * position_world};
		vertices.positions_image[i] = position_image / position_image(3);
	}
}

Uint32 packColorArgb(Uint32 a, Uint32 r, Uint32 g, Uint32 b)
{
	return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

void pixelShader(const Vertex& vertex, Pixels& pixels, size_t index)
{
	using namespace vertex_index;
	
	if (vertex(BARY0) < 0.0 || 1.0 < vertex(BARY0)) return;
	if (vertex(BARY1) < 0.0 || 1.0 < vertex(BARY1)) return;
	if (vertex(BARY2) < 0.0 || 1.0 < vertex(BARY2)) return;	

	const double disparity = vertex(DISPARITY);
	// TODO: try if defered rendering is faster.
	if (disparity > pixels.disparities[index])
	{
		//const double illumination = 0.5 * vertex(ILLUMINATION);
		const double illumination = 500 * vertex(DISPARITY);
		const Uint32 c = static_cast<Uint32>(clamp(illumination, 0.0, 255.0));
		pixels.colors[index] = packColorArgb(255, c, c, c);
		pixels.disparities[index] = disparity;
	}
}

void drawPoint(Pixels& pixels, const Vector4d& vertex_image)
{
    const auto x = static_cast<int>(vertex_image.x());
    const auto y = static_cast<int>(vertex_image.y());
    const auto disparity = vertex_image.z();

    if (0 <= x && x < pixels.width && 0 <= y && y < pixels.height && disparity > 0.0)
    {
		pixels.colors[y * pixels.width + x] = 0xFFFF00FF;
    }
}

void drawPoints(Pixels& pixels, const Vectors4d& vertices_image)
{
    for (const auto& vertex_image : vertices_image)
    {
        drawPoint(pixels, vertex_image);
    }
}

void drawTriangles(Pixels& pixels, const Vertices& vertices, const Triangles& triangles)
{
	const auto num_triangles = triangles.size();

	fill(pixels.disparities, 0.0);
	fill(pixels.colors, 0);

    for (size_t i = 0; i < num_triangles; ++i)
    {
        const auto i0 = triangles.indices0[i];
        const auto i1 = triangles.indices1[i];
        const auto i2 = triangles.indices2[i];

        const auto& v0 = vertices.positions_image[i0];
        const auto& v1 = vertices.positions_image[i1];
        const auto& v2 = vertices.positions_image[i2];

		if (isBehindCamera(v0, v1, v2)) continue;

        auto vertex0 = Vertex();
		auto vertex1 = Vertex();
		auto vertex2 = Vertex();

		using namespace vertex_index;

		vertex0(BARY0) = 1.0;
		vertex0(BARY1) = 0.0;
		vertex0(BARY2) = 0.0;
		vertex0(DISPARITY) = v0(2);

		vertex1(BARY0) = 0.0;
		vertex1(BARY1) = 1.0;
		vertex1(BARY2) = 0.0;
		vertex1(DISPARITY) = v1(2);

		vertex2(BARY0) = 0.0;
		vertex2(BARY1) = 0.0;
		vertex2(BARY2) = 1.0;
		vertex2(DISPARITY) = v2(2);

        renderTriangleTemplate(pixels, pixelShader, v0, v1, v2, vertex0, vertex1, vertex2);
    }
	/*
    for (size_t i = 0; i < num_triangles; ++i)
    {
        const auto i0 = triangles.indices0[i];
        const auto i1 = triangles.indices1[i];
        const auto i2 = triangles.indices2[i];

        const auto& v0 = vertices.positions_image[i0];
        const auto& v1 = vertices.positions_image[i1];
        const auto& v2 = vertices.positions_image[i2];

        drawPoint(pixels, vertices.positions_image[i0]);
        drawPoint(pixels, vertices.positions_image[i1]);
        drawPoint(pixels, vertices.positions_image[i2]);
    }
	*/
}
