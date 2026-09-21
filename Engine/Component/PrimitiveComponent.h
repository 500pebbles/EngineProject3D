#pragma once
#include "SceneComponent.h"

class ENGINE_API UPrimitiveComponent : public USceneComponent
{
    TYPE_DECLARATIONS(UPrimitiveComponent, USceneComponent)
    
public:
    UPrimitiveComponent() = default;
    virtual ~UPrimitiveComponent() = default;
    
};