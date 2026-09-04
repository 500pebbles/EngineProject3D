#include "PlayerActor.h"
#include <Component/SceneComponent.h>
#include <Component/CameraComponent.h>
#include <Render/Renderer3D.h>

PlayerActor::PlayerActor(const Vector3& location)
    : super(location)
{
    sceneComponent = AddComponent<USceneComponent>();
    SetRootComponent(sceneComponent);
    
    cameraComponent = AddComponent<UCameraComponent>();
    cameraComponent->SetAttachParent(sceneComponent);

    cameraComponent->SetRelativeLocation(Vector3(0.0f, 0.0f, 0.0f));
    cameraComponent->SetTarget(Vector3::Zero);
}

PlayerActor::~PlayerActor()
{
}

void PlayerActor::BeginPlay()
{
    AActor::BeginPlay();
    
    Renderer3D::Get().SetCamera(cameraComponent);
}

void PlayerActor::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);
}
