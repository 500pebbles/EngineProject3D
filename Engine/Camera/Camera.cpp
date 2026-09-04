#include "Camera.h"

Camera::Camera(const Vector3& position, const Vector3& target)
    : _position(position), _target(target)
{
    const Vector3 worldUp    { 0.0f, 1.0f, 0.0f };

    _viewMatrix = Matrix4::LookAt(_position,_target,worldUp);
}