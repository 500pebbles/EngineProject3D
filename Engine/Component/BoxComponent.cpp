#include "BoxComponent.h"


// width, height 받지말고 Vector로 이미지를 넣으면 2줄될듯
UBoxComponent::UBoxComponent(const Vector2& inBoxExtent) 
    : boxExtent(inBoxExtent)
{
    
}