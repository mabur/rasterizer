#include "SDL.h"

#include "input.hpp"
#include "vector_space.hpp"

bool isLeftMouseButtonDown(Uint32 mouse_state)
{
    return static_cast<bool>(mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT));
}

bool isRightMouseButtonDown(Uint32 mouse_state)
{
    return static_cast<bool>(mouse_state & SDL_BUTTON(SDL_BUTTON_RIGHT));
}

CameraCoordinates handleInput(CameraCoordinates camera_coordinates)
{
    SDL_SetRelativeMouseMode(SDL_TRUE);

    auto mouse_x = 0;
    auto mouse_y = 0;

    const auto mouse = SDL_GetRelativeMouseState(&mouse_x, &mouse_y);
    const auto keyboard = SDL_GetKeyboardState(NULL);
    const auto velocity = 0.05;
    const auto angular_velocity = 0.2 * 2.0 * 3.14 / 360.0;

    camera_coordinates.yaw   += mouse_x * angular_velocity;
    camera_coordinates.pitch -= mouse_y * angular_velocity;
    camera_coordinates.pitch = clamp(camera_coordinates.pitch, -3.14 * 0.5, +3.14 * 0.5);

    auto velocity_camera = Vector4d{ 0.0, 0.0, 0.0, 0.0 };

    if (keyboard[SDL_SCANCODE_D])
    {
        velocity_camera(0) += velocity;
    }
    if (keyboard[SDL_SCANCODE_A])
    {
        velocity_camera(0) -= velocity;
    }
    if (keyboard[SDL_SCANCODE_W])
    {
        velocity_camera(2) += velocity;
    }
    if (keyboard[SDL_SCANCODE_S])
    {
        velocity_camera(2) -= velocity;
    }
    if (isLeftMouseButtonDown(mouse))
    {
        velocity_camera(1) += velocity;
    }
    if (isRightMouseButtonDown(mouse))
    {
        velocity_camera(1) -= velocity;
    }

    const auto world_from_camera = worldFromCamera(camera_coordinates);
    const auto velocity_world = Vector4d{ world_from_camera * velocity_camera };

    camera_coordinates.x += velocity_world(0);
    camera_coordinates.y += velocity_world(1);
    camera_coordinates.z += velocity_world(2);

    return camera_coordinates;
}
