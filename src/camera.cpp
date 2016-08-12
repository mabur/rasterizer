#include "camera.hpp"

#include <Eigen/Geometry>

Matrix4d worldFromCamera(const CameraCoordinates& coordinates)
{
    auto world_from_camera = Matrix4d{Matrix4d::Identity()};
    world_from_camera.col(3) << coordinates.x, coordinates.y, coordinates.z, 1.0;
    using namespace Eigen;
    auto R = Matrix3d{};
    const auto R_flip  = AngleAxisd(3.14151965, Vector3d::UnitX());
    const auto R_yaw   = AngleAxisd(coordinates.yaw, Vector3d::UnitY());
    const auto R_pitch = AngleAxisd(coordinates.pitch, Vector3d::UnitX());
    R = R_flip * R_yaw * R_pitch;
    world_from_camera.topLeftCorner<3, 3>() = R;
    return world_from_camera;
}

Matrix4d cameraFromWorld(const CameraCoordinates& coordinates)
{
    return worldFromCamera(coordinates).inverse();
}

Matrix4d imageFromCamera(int width, int height)
{
    const auto fx = 0.5 * height;
    const auto fy = 0.5 * height;
    const auto cx = 0.5 * width;
    const auto cy = 0.5 * height;

    auto image_from_camera = Matrix4d{};
    image_from_camera <<
        fx, 0.0, cx, 0.0,
        0.0, fy, cy, 0.0,
        0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 0.0;
    return image_from_camera;
}