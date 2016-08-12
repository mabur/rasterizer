
#include "camera.hpp"
#include "drawing.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include "sdl_wrappers.hpp"
#include "vector_space.hpp"

int main(int, char**)
{
    const auto window_title = "Rasterizer";
    const auto width = 800;
    const auto height = 450;
    const auto image_from_camera = imageFromCamera(width, height);
    const auto filepath = "F:/mabur/Programmering/C++/rasterizer/models/sibenik/sibenik.obj";

    auto vertices_world = Vectors4d{};
    auto triangles = Triangles{};
    loadModel(filepath, vertices_world, triangles);
    auto vertices_image = vertices_world;

    auto sdl = Sdl(window_title, width, height);
    auto camera_coordinates = CameraCoordinates{};

    while (noQuitMessage())
    {
        camera_coordinates = handleInput(camera_coordinates);
        const auto camera_from_world = cameraFromWorld(camera_coordinates);
        const auto image_from_world = Matrix4d{image_from_camera * camera_from_world};
        projectPoints(image_from_world, vertices_world, vertices_image);

        sdl.clear();
        //drawPoints(sdl, vertices_image);
        drawTriangles(sdl, vertices_image, triangles);
        sdl.update();
    }
    return 0;
}
