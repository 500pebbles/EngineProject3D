#include "CameraComponent.h"

Matrix4 UCameraComponent::GetViewMatrix() const
{
    return Matrix4::LookAt(GetWorldLocation(), target, Vector3::Up);
}