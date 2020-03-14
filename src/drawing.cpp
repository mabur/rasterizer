#include <algorithm>

#include <Eigen/Core>

#include "algorithm.hpp"
#include "drawing.hpp"
#include "drawing_template.hpp"

Light makeLight()
{
    auto light = Light{};
    light.position_world = Vector4d{ 0, -10, 0, 1 };
    light.power = Vector4d{ 1, 1, 1, 0 };
    return light;
}

bool isBehindCamera(const Vector4d& v0, const Vector4d& v1, const Vector4d& v2)
{
	return v0(2) <= 0 || v1(2) <= 0 || v2(2) <= 0;
}

void vertexShader(Vertices& vertices, const Environment& environment)
{
	const auto num_vertices = vertices.size();
    const auto image_from_camera = imageFromCamera(environment.intrinsics);
    const auto camera_from_world = cameraFromWorld(environment.extrinsics);
    const auto image_from_world = Matrix4d{ image_from_camera * camera_from_world };

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

void basicPixelShader(const Vertex& vertex, Pixels& pixels, size_t index)
{
	using namespace vertex_index;
	
	if (vertex(BARY0) < 0.0 || 1.0 < vertex(BARY0)) return;
	if (vertex(BARY1) < 0.0 || 1.0 < vertex(BARY1)) return;
	if (vertex(BARY2) < 0.0 || 1.0 < vertex(BARY2)) return;	

	const double disparity = vertex(DISPARITY);
	// TODO: try if defered rendering is faster.
    if (disparity <= pixels.disparities[index]) return;

	const Uint32 c = static_cast<Uint32>(clamp(500 * disparity, 0.0, 255.0));
	pixels.colors[index] = packColorArgb(255, c, c, c);
	pixels.disparities[index] = disparity;
}

Uint32 clampColor(double c)
{
    return static_cast<Uint32>(clamp(c, 0.0, 255.0));
}

struct PixelEnvironment
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    const Texture* surface_texture;
    Vector4d surface_normal_world;
    Vector4d light_position_world;
    Vector4d light_power;
};

struct PixelShader
{
    PixelEnvironment pixel_environment;
    void operator()(const Vertex& vertex, Pixels& pixels, size_t index) const
    {
        using namespace vertex_index;

        if (vertex(BARY0) < 0.0 || 1.0 < vertex(BARY0)) return;
        if (vertex(BARY1) < 0.0 || 1.0 < vertex(BARY1)) return;
        if (vertex(BARY2) < 0.0 || 1.0 < vertex(BARY2)) return;

        const double disparity = vertex(DISPARITY);
        // TODO: try if defered rendering is faster.
        if (disparity <= pixels.disparities[index]) return;

        const Uint32 c = clampColor(255 * 2 * disparity);
        pixels.colors[index] = packColorArgb(255, c, c, c);
        pixels.disparities[index] = disparity;

        if (pixel_environment.surface_texture->empty()) return;

        const auto x = vertex(X) / disparity;
        const auto y = vertex(Y) / disparity;
        const auto z = vertex(Z) / disparity;

        const auto u = vertex(U) / disparity;
        const auto v = vertex(V) / disparity;
        const auto position_world = Vector4d{x, y, z, 1};

        //const auto light = disparity;
        const auto light = 16.0 / (position_world - pixel_environment.light_position_world).squaredNorm();

        const auto color = pixel_environment.surface_texture->sample(u, v);
        const auto red   = clampColor(light * color(RED));
        const auto green = clampColor(light * color(GREEN));
        const auto blue  = clampColor(light * color(BLUE));
        pixels.colors[index] = packColorArgb(255, red, green, blue);
    }
};

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

Vector4d computeNormal(const Vector4d& v0, const Vector4d& v1, const Vector4d& v2)
{
    const auto a = Eigen::Vector3d(v1(0) - v0(0), v1(1) - v0(1), v1(2) - v0(2));
    const auto b = Eigen::Vector3d(v2(0) - v0(0), v2(1) - v0(1), v2(2) - v0(2));
    //const auto c = Eigen::Vector3d{a.cross(b).normalized()};
    //return {c(0), c(1), c(2), 0};
    return Vector4d::Zero();
}

void drawTriangles(Pixels& pixels, const Vertices& vertices,
    const Triangles& triangles, const Textures& textures, const Environment& environment)
{
	const auto num_triangles = triangles.size();

	fill(pixels.disparities, 0.0);
	fill(pixels.colors, 0);

    auto pixel_shader = PixelShader{};
    auto vertex0 = Vertex();
    auto vertex1 = Vertex();
    auto vertex2 = Vertex();

    for (size_t i = 0; i < num_triangles; ++i)
    {
        const auto i0 = triangles.indices0[i];
        const auto i1 = triangles.indices1[i];
        const auto i2 = triangles.indices2[i];

        const auto& v0 = vertices.positions_image[i0];
        const auto& v1 = vertices.positions_image[i1];
        const auto& v2 = vertices.positions_image[i2];

        const auto& t0 = vertices.positions_texture[i0];
        const auto& t1 = vertices.positions_texture[i1];
        const auto& t2 = vertices.positions_texture[i2];

        const auto& p0 = vertices.positions_world[i0];
        const auto& p1 = vertices.positions_world[i1];
        const auto& p2 = vertices.positions_world[i2];

		if (isBehindCamera(v0, v1, v2)) continue;

		using namespace vertex_index;

		vertex0(BARY0) = 1.0;
		vertex0(BARY1) = 0.0;
		vertex0(BARY2) = 0.0;
		vertex0(DISPARITY) = v0(2);
        vertex0(U) = t0(0) * v0(2);
        vertex0(V) = t0(1) * v0(2);
        vertex0(X) = p0(0) * v0(2);
        vertex0(Y) = p0(1) * v0(2);
        vertex0(Z) = p0(2) * v0(2);

		vertex1(BARY0) = 0.0;
		vertex1(BARY1) = 1.0;
		vertex1(BARY2) = 0.0;
		vertex1(DISPARITY) = v1(2);
        vertex1(U) = t1(0) * v1(2);
        vertex1(V) = t1(1) * v1(2);
        vertex1(X) = p1(0) * v1(2);
        vertex1(Y) = p1(1) * v1(2);
        vertex1(Z) = p1(2) * v1(2);

		vertex2(BARY0) = 0.0;
		vertex2(BARY1) = 0.0;
		vertex2(BARY2) = 1.0;
		vertex2(DISPARITY) = v2(2);
        vertex2(U) = t2(0) * v2(2);
        vertex2(V) = t2(1) * v2(2);
        vertex2(X) = p2(0) * v2(2);
        vertex2(Y) = p2(1) * v2(2);
        vertex2(Z) = p2(2) * v2(2);

        const auto texture_index = triangles.texture_indices[i];
        pixel_shader.pixel_environment.surface_texture = &textures[texture_index];
        pixel_shader.pixel_environment.light_position_world = environment.light.position_world;
        pixel_shader.pixel_environment.light_power = environment.light.power;

        //renderTriangleTemplate(pixels, basicPixelShader, v0, v1, v2, vertex0, vertex1, vertex2);
        renderTriangleTemplate(pixels, pixel_shader, v0, v1, v2, vertex0, vertex1, vertex2);
    }
}
