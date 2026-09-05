#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>

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
    void ProcessInput(float deltaTime);
    void Move(float forwardInput, float rightInput, float verticalInput, float deltaTime);
    void Look(float deltaX, float deltaY);

private:
    std::shared_ptr<USceneComponent> sceneComponent;
    std::shared_ptr<UCameraComponent> cameraComponent;
    
    float yaw = 0.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.00025f;

    bool isMouseInitialized = false;
    
};
