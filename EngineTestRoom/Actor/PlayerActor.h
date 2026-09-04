#pragma once
#include <Actor/Actor.h>

class UCameraComponent;

class PlayerActor : public AActor
{
    TYPE_DECLARATIONS(PlayerActor, AActor)

public:
    PlayerActor(const Vector3& location);
    ~PlayerActor();
    
    void BeginPlay() override;
    void Tick(float deltaTime) override;

private:
    std::shared_ptr<USceneComponent> sceneComponent;
    std::shared_ptr<UCameraComponent> cameraComponent;
};
