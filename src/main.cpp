#define SDL_MAIN_HANDLED

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
    const auto width = 800;// 640;
    const auto height = 600;// 360;
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

    const auto light = makeLight();
    const auto intrinsics = makeCameraIntrinsics(width, height);
    auto extrinsics = CameraExtrinsics{};
	auto buffers = Pixels(width, height);
	auto sdl = Sdl(window_title, width, height);
    auto environment = Environment{ intrinsics, extrinsics, light };

    while (noQuitMessage())
    {    
        environment = handleInput(environment);
		vertexShader(vertices, environment);
		drawTriangles(buffers, vertices, triangles, textures, environment);
		sdl.setPixels(buffers.colors.data());
        sdl.update();
    }
    return 0;
}
