#pragma once

#include "PrimitiveComponent.h"

class ENGINE_API UBoxComponent : public UPrimitiveComponent
{
    TYPE_DECLARATIONS(UBoxComponent, UPrimitiveComponent)
    
public:
    UBoxComponent(const Vector2& inBoxExtent);
    virtual ~UBoxComponent() = default;
    
    inline Vector2 GetBoxExtent() const { return boxExtent; }
    inline void SetBoxExtent(Vector2 inBoxExtent) { boxExtent = inBoxExtent; }

    inline float GetBoxWidth() const { return boxExtent.x; }
    inline float GetBoxHeight() const { return boxExtent.y; }
    
protected:
    Vector2 boxExtent;
};
