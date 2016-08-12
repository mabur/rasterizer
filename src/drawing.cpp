#include "drawing.hpp"

double barycentric(
    const Vector4d& vertex_right, const Vector4d& vertex_left, const Vector4d& point)
{
    return (vertex_left(0) - vertex_right(0)) * (point(1) - vertex_right(1))
         - (vertex_left(1) - vertex_right(1)) * (point(0) - vertex_right(0));
}

double min3(double a, double b, double c)
{
    return std::min(a, std::min(b, c));
}

double max3(double a, double b, double c)
{
    return std::max(a, std::max(b, c));
}

using Vertex = double;
using Pixel = Uint32;
Pixel pixel_shader(const Vertex& vertex, Pixel pixel)
{
    //*pixel = pixel_shader(vertex, *pixel);
    return 0xFF00FF00;
}

void renderTriangleTemplate(Sdl& sdl,
    const Vector4d& v0, const Vector4d& v1, const Vector4d& v2,
    const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2)
{
    if (v0(2) <= 0 || v1(2) <= 0 || v2(2) <= 0) return;

    const auto width    = static_cast<size_t>(sdl.width);
    const auto width_d  = static_cast<double>(sdl.width);
    const auto height_d = static_cast<double>(sdl.height);

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

    // TODO: ceil and floor?
    const auto x_min_i = static_cast<size_t>(x_min);
    const auto x_max_i = static_cast<size_t>(x_max);
    const auto y_min_i = static_cast<size_t>(y_min);
    const auto y_max_i = static_cast<size_t>(y_max);
    
    const auto p  = Vector4d(x_min, y_min, 0.0, 0.0);
    const auto dx = Vector4d(1.0, 0.0, 0.0, 0.0);
    const auto dy = Vector4d(0.0, 1.0, 0.0, 0.0);

    const auto w0_row = barycentric(v1, v2, p);
    const auto w1_row = barycentric(v2, v0, p);
    const auto w2_row = barycentric(v0, v1, p);

    const auto w0_dx = barycentric(v1, v2, p + dx) - barycentric(v1, v2, p);
    const auto w1_dx = barycentric(v2, v0, p + dx) - barycentric(v2, v0, p);
    const auto w2_dx = barycentric(v0, v1, p + dx) - barycentric(v0, v1, p);

    const auto w0_dy = barycentric(v1, v2, p + dy) - barycentric(v1, v2, p);
    const auto w1_dy = barycentric(v2, v0, p + dy) - barycentric(v2, v0, p);
    const auto w2_dy = barycentric(v0, v1, p + dy) - barycentric(v0, v1, p);

    const auto c = 1.0 / barycentric(v0, v1, v2);

    //using Vertex = typename Eigen::Matrix<T, VERTEX_SIZE, 1>;

    const Vertex vertex_row = c * (w0_row * vertex0 + w1_row * vertex1 + w2_row * vertex2);
    const Vertex vertex_dx  = c * (w0_dx  * vertex0 + w1_dx  * vertex1 + w2_dx  * vertex2);
    const Vertex vertex_dy  = c * (w0_dy  * vertex0 + w1_dy  * vertex1 + w2_dy  * vertex2);

    auto pixel_current_row = sdl.pixels.data() + y_min_i * width + x_min_i;
    auto vertex_current_row = vertex_row;

    for (auto y = y_min_i; y <= y_max_i; ++y)
    {
        auto vertex = vertex_current_row;
        auto pixel  = pixel_current_row;

        for (auto x = x_min_i; x <= x_max_i; ++x)
        {
            *pixel = pixel_shader(vertex, *pixel);

            vertex += vertex_dx;
            ++pixel;
        }

        vertex_current_row += vertex_dy;
        pixel_current_row += width;
    }
}

void drawPoint(Sdl& sdl, const Vector4d& vertex_image)
{
    const auto x = static_cast<int>(vertex_image.x());
    const auto y = static_cast<int>(vertex_image.y());
    const auto disparity = vertex_image.z();

    if (0 <= x && x < sdl.width && 0 <= y && y < sdl.height && disparity > 0.0)
    {
        sdl.pixels[y * sdl.width + x] = 0xFFFF00FF;
    }
}

void drawPoints(Sdl& sdl, const Vectors4d& vertices_image)
{
    for (const auto& vertex_image : vertices_image)
    {
        drawPoint(sdl, vertex_image);
    }
}

void drawTriangles(
    Sdl& sdl,
    const Vectors4d& vertices_image,
    const Triangles& triangles)
{
    const auto num_triangles = triangles.indices0.size();

    for (size_t i = 0; i < num_triangles; ++i)
    {
        const auto i0 = triangles.indices0[i];
        const auto i1 = triangles.indices1[i];
        const auto i2 = triangles.indices2[i];

        const auto& v0 = vertices_image[i0];
        const auto& v1 = vertices_image[i1];
        const auto& v2 = vertices_image[i2];

        double vertex0 = 0;
        double vertex1 = 1;
        double vertex2 = 2;

        renderTriangleTemplate(sdl, v0, v1, v2, vertex0, vertex1, vertex2);
    }

    for (size_t i = 0; i < num_triangles; ++i)
    {
        const auto i0 = triangles.indices0[i];
        const auto i1 = triangles.indices1[i];
        const auto i2 = triangles.indices2[i];

        const auto& v0 = vertices_image[i0];
        const auto& v1 = vertices_image[i1];
        const auto& v2 = vertices_image[i2];

        double vertex0 = 0;
        double vertex1 = 1;
        double vertex2 = 2;

        drawPoint(sdl, vertices_image[i0]);
        drawPoint(sdl, vertices_image[i1]);
        drawPoint(sdl, vertices_image[i2]);
    }
}
