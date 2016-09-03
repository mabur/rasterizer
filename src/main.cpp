
#include "algorithm.hpp"
#include "camera.hpp"
#include "drawing.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "sdl_wrappers.hpp"
#include "vector_space.hpp"

int main(int, char**)
{
    const auto window_title = "Rasterizer";
	const auto width = 640;
	const auto height = 360;
    const auto filepath = "../../models/sibenik/sibenik.obj";

    auto vertices_world = Vectors4d{};
    auto triangles = Triangles{};
    loadModel(filepath, vertices_world, triangles);
	const auto num_vertices = vertices_world.size();
	auto vertices = Vertices(num_vertices);
	vertices.positions_world = vertices_world;

	const auto image_from_camera = imageFromCamera(width, height);
    auto camera_coordinates = CameraCoordinates{};
	auto environment = Environment{};
	auto buffers = Pixels(width, height);
	auto sdl = Sdl(window_title, width, height);

    while (noQuitMessage())
    {
        camera_coordinates = handleInput(camera_coordinates);
        const auto camera_from_world = cameraFromWorld(camera_coordinates);
		environment.image_from_world = image_from_camera * camera_from_world;

		vertexShader(vertices, environment);
		drawTriangles(buffers, vertices, triangles);
		// Merge these to one?
        sdl.clear();
		sdl.setPixels(buffers.colors.data());
        sdl.update();
    }
    return 0;
}
