#pragma once

#include "PrimitiveComponent.h"

class ENGINE_API UBoxComponent : public UPrimitiveComponent
{
    TYPE_DECLARATIONS(UBoxComponent, UPrimitiveComponent)
    
public:
    UBoxComponent() = default;
    virtual ~UBoxComponent() = default;
    
    inline Vector3 GetBoxExtent() const { return boxExtent; }
    inline void SetBoxExtent(Vector3 inBoxExtent) { boxExtent = inBoxExtent; }

    float GetBoxWidth() const { return boxExtent.x * 2.0f; }
    float GetBoxHeight() const { return boxExtent.y * 2.0f; }
    float GetBoxDepth() const { return boxExtent.z * 2.0f; }
    
protected:
    Vector3 boxExtent = Vector3(.5f, .5f, .5f);   // 언리얼식 BoxExtent는 원점에서부터 변까지의 거리 (x0.5)
};
