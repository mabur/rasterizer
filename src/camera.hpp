#pragma once

#include "vector_space.hpp"

struct CameraExtrinsics
{
    double x;
    double y;
    double z;
    double yaw;
    double pitch;
};

struct CameraIntrinsics
{
    double fx;
    double fy;
    double cx;
    double cy;
    size_t width;
    size_t height;
};

CameraIntrinsics makeCameraIntrinsics(size_t width, size_t height);

Matrix4d imageFromCamera(const CameraIntrinsics& intrinsics);
Matrix4d worldFromCamera(const CameraExtrinsics& coordinates);
Matrix4d cameraFromWorld(const CameraExtrinsics& coordinates);
