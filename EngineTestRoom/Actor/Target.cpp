#include "Target.h"

#include <Component/MeshRendererComponent.h>
#include <Component/SphereComponent.h>

#include "Stalker.h"

Target::Target(const Vector3& location)
: super(location)
{
    sphereComponent = AddComponent<USphereComponent>();
    meshRendererComponent = AddComponent<UMeshRendererComponent>();   
    SetRootComponent(meshRendererComponent);
     
    sphereComponent->SetRelativeLocation(location);
    meshRendererComponent->SetMeshColor(Color::BrightGreen);
    SetActorScale(Vector3(0.8f, 0.8f, 0.8f));
}

Target::~Target()
{
}

void Target::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);
    SetActorRotation(GetActorRotation() + Vector3(1.f,1.f,0.f));
}

void Target::OnCollision(const std::shared_ptr<AActor>& other)
{
    AActor::OnCollision(other);
    if (other->IsTypeOf<Stalker>()) Destroy();
}
