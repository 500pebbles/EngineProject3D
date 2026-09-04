#include "Character.h"
#include <Math/Color.h>
#include <Component/BoxComponent.h>
#include <Component/SpriteRendererComponent.h>

ACharacter::ACharacter(const Vector3& location)
    : super(location)
{
    collisionComponent = AddComponent<UBoxComponent>(Vector2(1.f, 1.f));
    SetRootComponent(collisionComponent);
    
    spriteComponent = AddComponent<USpriteRendererComponent>("This Shouldn't be displayed...", Color::Red, 0);
}
