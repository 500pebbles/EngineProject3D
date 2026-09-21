#pragma once
#include <Actor/Actor.h>

class USphereComponent;
class UMeshRendererComponent;

class Terrain : public AActor
{
    TYPE_DECLARATIONS(Terrain, AActor)

public:
    Terrain(const Vector3& location);
    ~Terrain();
    
    void Tick(float deltaTime) override;    
    
private:
    std::shared_ptr<UMeshRendererComponent> meshRendererComponent;
};
