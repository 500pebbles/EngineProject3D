#pragma once

#include <string>

#include "ActorComponent.h"
#include <Math/Color.h>
#include "PrimitiveComponent.h"

class RenderPosition;

class ENGINE_API USpriteRendererComponent : public UPrimitiveComponent
{
    TYPE_DECLARATIONS(USpriteRendererComponent, UPrimitiveComponent)
        
public:
    USpriteRendererComponent(
        const std::string& image = "",
        Color color = Color::White,
        int sortingOrder = 0
        );
    
    virtual ~USpriteRendererComponent() = default;
    
    virtual void Draw() override;
        
    void Submit(const RenderPosition& position);
        
    /* Character */
    inline const std::string& GetImage() const { return image;}
    inline void SetImage(const std::string& newImage) { image = newImage;}
        
    /* Color */
    inline Color GetColor() const { return color; }
    inline void SetColor(Color newColor) { color = newColor; }
        
    /* Order */
    inline int GetSortingOrder() const { return sortingOrder; }
    inline void SetSortingOrder(int newSortingOrder) { sortingOrder = newSortingOrder; }
        
protected:
    std::string image;        
    Color color = Color::White;        
    int sortingOrder = 0;
};
