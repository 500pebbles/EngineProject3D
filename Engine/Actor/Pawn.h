#pragma once
#include "Actor.h"

class USpriteRendererComponent;
class UBoxComponent;

class ENGINE_API APawn : public AActor
{
    TYPE_DECLARATIONS(APawn, AActor)
    
public:
    APawn(const Vector3& location);
    
protected:
};
