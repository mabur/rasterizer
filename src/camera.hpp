#pragma once

#include "vector_space.hpp"

struct CameraCoordinates
{
    double x;
    double y;
    double z;
    double yaw;
    double pitch;
};

Matrix4d imageFromCamera(int width, int height);
Matrix4d worldFromCamera(const CameraCoordinates& coordinates);
Matrix4d cameraFromWorld(const CameraCoordinates& coordinates);
