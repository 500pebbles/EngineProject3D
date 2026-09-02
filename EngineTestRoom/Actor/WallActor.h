#pragma once
#include <Actor/Actor.h>
#include <Actor/Actor.h>

class WallActor : public AActor
{
    TYPE_DECLARATIONS(WallActor, AActor)
public:
    WallActor(const Vector2& location);
    
    void Tick(float deltaTime) override;
    
    void OnCollision(const std::shared_ptr<AActor>& other) override;
    
    void Destroy() override;
    
};
