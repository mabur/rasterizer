#pragma once

#include "vector_space.hpp"

inline double barycentric(
	const Vector4d& vertex_right, const Vector4d& vertex_left, const Vector4d& point)
{
	return (vertex_left(0) - vertex_right(0)) * (point(1) - vertex_right(1))
		 - (vertex_left(1) - vertex_right(1)) * (point(0) - vertex_right(0));
}

template<typename Pixels, typename PixelShader, typename Vertex>
void renderTriangleTemplate(
	Pixels& pixels, PixelShader pixel_shader,
	const Vector4d& v0, const Vector4d& v1, const Vector4d& v2,
	const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
{
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

	const auto p       = Vector4d(x_min,       y_min, 0.0, 0.0);
	const auto p_right = Vector4d(x_min + 1.0, y_min, 0.0, 0.0);
	const auto p_down  = Vector4d(x_min, y_min + 1.0, 0.0, 0.0);

	const auto w0_row = barycentric(v1, v2, p);
	const auto w1_row = barycentric(v2, v0, p);
	const auto w2_row = barycentric(v0, v1, p);

	const auto w0_dx = barycentric(v1, v2, p_right) - barycentric(v1, v2, p);
	const auto w1_dx = barycentric(v2, v0, p_right) - barycentric(v2, v0, p);
	const auto w2_dx = barycentric(v0, v1, p_right) - barycentric(v0, v1, p);

	const auto w0_dy = barycentric(v1, v2, p_down)  - barycentric(v1, v2, p);
	const auto w1_dy = barycentric(v2, v0, p_down)  - barycentric(v2, v0, p);
	const auto w2_dy = barycentric(v0, v1, p_down)  - barycentric(v0, v1, p);

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
		auto index = index_current_row;

		for (auto x = x_min_i; x <= x_max_i; ++x)
		{
			pixel_shader(vertex, pixels, index);
			vertex += vertex_dx;
			++index;
		}

		vertex_current_row += vertex_dy;
		index_current_row += width;
	}
}
