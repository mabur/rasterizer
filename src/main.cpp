
#include "algorithm.hpp"
#include "camera.hpp"
#include "drawing.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "sdl_wrappers.hpp"
#include "texture.hpp"
#include "vector_space.hpp"

int main(int, char**)
{
    const auto window_title = "Rasterizer";
    const auto width = 1024;// 640;
    const auto height = 768;// 360;
    //const auto filepath = "../../models/kapell_2017.obj";
    const auto filepath = "../../models/sibenik/sibenik.obj";

    auto positions_world = Vectors4d{};
    auto positions_texture = Vectors2d{};
    auto triangles = Triangles{};
    auto textures = Textures{};
    loadModel(filepath, positions_world, positions_texture, triangles, textures);
	const auto num_vertices = positions_world.size();
	auto vertices = Vertices(num_vertices);
	vertices.positions_world = positions_world;
    vertices.positions_texture = positions_texture;

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
		drawTriangles(buffers, vertices, triangles, textures);
		// Merge these to one?
        sdl.clear();
		sdl.setPixels(buffers.colors.data());
        sdl.update();
    }
    return 0;
}
