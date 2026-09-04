#pragma once
#include <Core/Core.h>
#include "Actor.h"
#include "Component/CameraComponent.h"

class ACameraActor : public AActor
{
public:
    TYPE_DECLARATIONS(ACameraActor, AActor)
  
public:
    ACameraActor(const Vector3& location);
    
private:
    std::shared_ptr<UCameraComponent> cameraComponent;
};
