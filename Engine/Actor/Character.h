#pragma once
#include "Pawn.h"

class ENGINE_API ACharacter : public APawn
{
    TYPE_DECLARATIONS(ACharacter, APawn)
    
public:
    ACharacter(const Vector2& location);        
    
protected:    
    std::shared_ptr<UBoxComponent> collisionComponent;
    std::shared_ptr<USpriteRendererComponent> spriteComponent;
};