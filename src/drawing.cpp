#include <algorithm>

#include <Eigen/Core>

#include "algorithm.hpp"
#include "drawing.hpp"

double barycentric(
    const Vector4d& vertex_right, const Vector4d& vertex_left, const Vector4d& point)
{
    return (vertex_left(0) - vertex_right(0)) * (point(1) - vertex_right(1))
         - (vertex_left(1) - vertex_right(1)) * (point(0) - vertex_right(0));
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

bool isBehindCamera(const Vector4d& v0, const Vector4d& v1, const Vector4d& v2)
{
	return v0(2) <= 0 || v1(2) <= 0 || v2(2) <= 0;
}

void renderTriangleTemplate(
	Pixels& pixels,
    const Vector4d& v0, const Vector4d& v1, const Vector4d& v2,
    const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
{
	if (isBehindCamera(v0, v1, v2)) return;

    const auto width    = pixels.width;
    const auto width_d  = static_cast<double>(pixels.width);
    const auto height_d = static_cast<double>(pixels.height);

    auto x_min = min3(v0.x(), v1.x(), v2.x());
    auto x_max = max3(v0.x(), v1.x(), v2.x());
    auto y_min = min3(v0.y(), v1.y(), v2.y());
    auto y_max = max3(v0.y(), v1.y(), v2.y());

    if (x_max < 0.0 || y_max < 0.0) return;

    if (width_d - 1.0 < x_min || height_d - 1.0 < y_min) return;

    x_min = clamp(floor(x_min), 0.0, width_d  - 1.0);
    x_max = clamp( ceil(x_max), 0.0, width_d  - 1.0);
    y_min = clamp(floor(y_min), 0.0, height_d - 1.0);
    y_max = clamp( ceil(y_max), 0.0, height_d - 1.0);

    const auto x_min_i = static_cast<size_t>(x_min);
    const auto x_max_i = static_cast<size_t>(x_max);
    const auto y_min_i = static_cast<size_t>(y_min);
    const auto y_max_i = static_cast<size_t>(y_max);
    
    const auto p       = Vector4d(x_min      , y_min      , 0.0, 0.0);
	const auto p_right = Vector4d(x_min + 1.0, y_min      , 0.0, 0.0);
	const auto p_down  = Vector4d(x_min      , y_min + 1.0, 0.0, 0.0);

    const auto w0_row = barycentric(v1, v2, p);
    const auto w1_row = barycentric(v2, v0, p);
    const auto w2_row = barycentric(v0, v1, p);

    const auto w0_dx = barycentric(v1, v2, p_right) - barycentric(v1, v2, p);
    const auto w1_dx = barycentric(v2, v0, p_right) - barycentric(v2, v0, p);
    const auto w2_dx = barycentric(v0, v1, p_right) - barycentric(v0, v1, p);

    const auto w0_dy = barycentric(v1, v2, p_down) - barycentric(v1, v2, p);
    const auto w1_dy = barycentric(v2, v0, p_down) - barycentric(v2, v0, p);
    const auto w2_dy = barycentric(v0, v1, p_down) - barycentric(v0, v1, p);

    const auto c = 1.0 / barycentric(v0, v1, v2);

    //using Vertex = typename Eigen::Matrix<T, VERTEX_SIZE, 1>;

    const Vertex vertex_row = c * (w0_row * vertex0 + w1_row * vertex1 + w2_row * vertex2);
    const Vertex vertex_dx  = c * (w0_dx  * vertex0 + w1_dx  * vertex1 + w2_dx  * vertex2);
    const Vertex vertex_dy  = c * (w0_dy  * vertex0 + w1_dy  * vertex1 + w2_dy  * vertex2);

    auto index_current_row = y_min_i * width + x_min_i;
    auto vertex_current_row = vertex_row;

    for (auto y = y_min_i; y <= y_max_i; ++y)
    {
        auto vertex = vertex_current_row;
        auto index  = index_current_row;

        for (auto x = x_min_i; x <= x_max_i; ++x)
        {
            pixelShader(vertex, pixels, index);

            vertex += vertex_dx;
            ++index;
        }

        vertex_current_row += vertex_dy;
        index_current_row += width;
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

        renderTriangleTemplate(pixels, v0, v1, v2, vertex0, vertex1, vertex2);
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
