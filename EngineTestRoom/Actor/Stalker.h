#pragma once
#include <Actor/Actor.h>

class USphereComponent;
class UMeshRendererComponent;

class Stalker : public AActor
{
    TYPE_DECLARATIONS(Stalker, AActor)

public:
    Stalker(const Vector3& location);
    ~Stalker();
    
    void Tick(float deltaTime);   
    void OnCollision(const std::shared_ptr<AActor>& other) override;
    
private:
    std::shared_ptr<UMeshRendererComponent> meshRendererComponent;
    std::shared_ptr<USphereComponent> sphereComponent;
};
