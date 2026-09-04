#pragma once

#include <Math/Matrix4.h>
#include <Math/Vector3.h>
#include <Core/Core.h>
class ENGINE_API Camera
{
public:
    Camera(const Vector3& position, const Vector3& target);

    const Matrix4& GetViewMatrix() const
    {
        return _viewMatrix;
    }

private:
    Vector3 _position;
    Vector3 _target;

    Matrix4 _viewMatrix;
};