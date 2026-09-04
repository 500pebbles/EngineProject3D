#include "CameraActor.h"

ACameraActor::ACameraActor(const Vector3& location)
    : AActor(location)
{
    cameraComponent = AddComponent<UCameraComponent>();
    SetRootComponent(cameraComponent);
}