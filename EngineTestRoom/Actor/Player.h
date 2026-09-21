#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>

class UCameraComponent;

class Player : public AActor
{
    TYPE_DECLARATIONS(Player, AActor)

public:
    Player(const Vector3& location);
    ~Player();
    
    void BeginPlay() override;
    void Tick(float deltaTime) override;
    
private:
    void ProcessInput(float deltaTime);
    void Move(float forwardInput, float rightInput, float verticalInput, float deltaTime);
    void Look(float deltaX, float deltaY);

private:
    std::shared_ptr<UCameraComponent> cameraComponent;
    
    float yaw = 0.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.00025f;

    bool isMouseInitialized = false;
    
};
