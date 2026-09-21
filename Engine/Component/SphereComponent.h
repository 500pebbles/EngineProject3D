#pragma once

#include "PrimitiveComponent.h"

class ENGINE_API USphereComponent : public UPrimitiveComponent
{
    TYPE_DECLARATIONS(USphereComponent, UPrimitiveComponent)

public:
    USphereComponent() = default;
    ~USphereComponent() = default;
    
    inline float GetRadius() const { return sphereRadius; }
    inline void SetRadius(float radius) { sphereRadius = radius; }
    
protected:
    float sphereRadius = 0.5f; 
    
};
