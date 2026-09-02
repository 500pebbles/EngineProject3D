#pragma once
#include <Actor/Actor.h>
#include <Actor/Character.h>

class TestActor : public ACharacter
{
    TYPE_DECLARATIONS(TestActor, ACharacter)

public:
    TestActor(const Vector2& location);
    
    void Tick(float deltaTime) override;
    
private:		
    float positionX = 0.0f;
    float positionY = 0.0f;
    float directionX = 1.0f;  
    float directionY = 0.0f;
    
private:
    float maxMoveSpeed = 10.f;
    float moveSpeed = 10.f;   
    
    void Move(float moveDirectionX, float moveDirectionY, float deltaTime);
};
