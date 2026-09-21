#pragma once
#include <Actor/Actor.h>

class USphereComponent;
class UMeshRendererComponent;

class Target : public AActor
{
    TYPE_DECLARATIONS(Target, AActor)
    
public:
    Target(const Vector3& location);
    ~Target();
    
    void Tick(float deltaTime);   
    void OnCollision(const std::shared_ptr<AActor>& other) override;
    
private:
    std::shared_ptr<UMeshRendererComponent> meshRendererComponent;
    std::shared_ptr<USphereComponent> sphereComponent;
};
