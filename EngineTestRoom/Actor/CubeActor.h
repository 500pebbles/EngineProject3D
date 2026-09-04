#pragma once

#include <Actor/Actor.h>

class UMeshRendererComponent;

class CubeActor : public AActor
{
    TYPE_DECLARATIONS(CubeActor, AActor)

public:
    CubeActor(const Vector3& location);
    
    void Tick(float deltaTime) override;
    
private:
    std::shared_ptr<UMeshRendererComponent> meshRendererComponent;
};
