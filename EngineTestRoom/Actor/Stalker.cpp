#include "Stalker.h"

#include <Component/MeshRendererComponent.h>
#include <Component/SphereComponent.h>

Stalker::Stalker(const Vector3& location)
    : super(location)
{
    sphereComponent = AddComponent<USphereComponent>();
    meshRendererComponent = AddComponent<UMeshRendererComponent>();   
    SetRootComponent(meshRendererComponent);
     
    meshRendererComponent->SetRelativeLocation(location);    
    meshRendererComponent->SetMeshColor(Color::BrightRed);
    SetActorScale(Vector3(0.8f, 0.8f, 0.8f));
}

Stalker::~Stalker()
{
}

void Stalker::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);
    SetActorRotation(GetActorRotation() + Vector3(-1.f,-1.f,0.f));

}

void Stalker::OnCollision(const std::shared_ptr<AActor>& other)
{
    AActor::OnCollision(other);
}
