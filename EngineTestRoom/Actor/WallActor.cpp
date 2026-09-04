#include "WallActor.h"

#include <Component/BoxComponent.h>
#include <Component/SpriteRendererComponent.h>
#include <Engine/Engine.h>
#include <Math/Color.h>
#include "TestActor.h"
#include <Render/Renderer.h>

class USpriteRendererComponent;

WallActor::WallActor(const Vector2& location)
    : super(location)
{
    std::shared_ptr<UBoxComponent> root = AddComponent<UBoxComponent>(Vector2(1.f, 1.f));    
    SetRootComponent(root);
    
    AddComponent<USpriteRendererComponent>("#",Color::Blue,0);
    
}

void WallActor::Tick(float deltaTime)
{    
    AActor::Tick(deltaTime);
    Renderer::Get().Submit(std::to_string(GetActorLocation().x), RenderPosition(0,0), Color::Blue, 5);
    Renderer::Get().Submit(std::to_string(GetActorLocation().y), RenderPosition(0,1), Color::Blue, 5);
}

void WallActor::OnCollision(const std::shared_ptr<AActor>& other)
{
    AActor::OnCollision(other);
    
    if (other->IsTypeOf<TestActor>())
    {
        
        Destroy();
        other->Destroy();
    }
} 

void WallActor::Destroy()
{
    AActor::Destroy();
}
