#include "TestActor.h"

#include <iostream>
#include <Component/BoxComponent.h>
#include <Component/SpriteRendererComponent.h>
#include <Input/Input.h>
#include <Render/Renderer.h>

TestActor::TestActor(const Vector2& location)
    : super(location)
{
    spriteComponent->SetImage("P");
    collisionComponent->SetBoxExtent(Vector2(1.f, 1.f));
    
    positionX = GetActorLocation().x;
    positionY = GetActorLocation().y;    
}

void TestActor::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);

    Renderer::Get().Submit(std::to_string(GetActorLocation().x), RenderPosition(0,5), Color::Red, 5);
    Renderer::Get().Submit(std::to_string(GetActorLocation().y), RenderPosition(0,6), Color::Red, 5);

    
    /* Movement Section */
    float moveDirectionX = 0.0f;
    float moveDirectionY = 0.0f;

    if (Input::Get().GetKey(VK_RIGHT))
    {
        moveDirectionX = 1.0f;
    }
    if (Input::Get().GetKey(VK_LEFT))
    {
        moveDirectionX = -1.0f;
    }
    if (Input::Get().GetKey(VK_UP))
    {
        moveDirectionY = -1.0f;
    }
    if (Input::Get().GetKey(VK_DOWN))
    {
        moveDirectionY = 1.0f;
    }

    Move(moveDirectionX, moveDirectionY, deltaTime);

    /* Weapon Section */
    if (Input::Get().GetKey(VK_SPACE))
    {
        std::cout << GetActorLocation().x << std::endl;
    }

    /* Direction Section */
    if (Input::Get().GetKey(VK_RIGHT))
    {
        directionX = 1.f;
        directionY = 0.f;
    }
    if (Input::Get().GetKey(VK_LEFT))
    {
        directionX = -1.f;
        directionY = 0.f;
    }
    if (Input::Get().GetKey(VK_UP))
    {
        directionX = 0.f;
        directionY = -1.f;
    }
    if (Input::Get().GetKey(VK_DOWN))
    {
        directionX = 0.f;
        directionY = 1.f;
    }
    if (Input::Get().GetKey(VK_RIGHT) && Input::Get().GetKey(VK_UP))
    {
        directionX = 1.f;
        directionY = -1.f;
    }
    if (Input::Get().GetKey(VK_RIGHT) && Input::Get().GetKey(VK_DOWN))
    {
        directionX = 1.f;
        directionY = 1.f;
    }
    if (Input::Get().GetKey(VK_LEFT) && Input::Get().GetKey(VK_UP))
    {
        directionX = -1.f;
        directionY = -1.f;
    }
    if (Input::Get().GetKey(VK_LEFT) && Input::Get().GetKey(VK_DOWN))
    {
        directionX = -1.f;
        directionY = 1.f;
    }
}

void TestActor::Move(float moveDirectionX, float moveDirectionY, float deltaTime)
{
    positionX = positionX + moveDirectionX * moveSpeed * deltaTime;
    positionY = positionY + moveDirectionY * moveSpeed * deltaTime; 

    Vector2 newPosition;
    newPosition.x = positionX;
    newPosition.y = positionY;
    SetActorLocation(newPosition);    
}
