#include "SceneComponent.h"

USceneComponent::USceneComponent(const Vector2& relativeLocation)
    : relativeLocation(relativeLocation), previousComponentLocation(relativeLocation)
{
}

Vector2 USceneComponent::GetComponentLocation() const
{
    std::shared_ptr<USceneComponent> parent = attachParent.lock();
    if (!parent) return relativeLocation;
    
    return parent->GetComponentLocation() + relativeLocation;
}

void USceneComponent::SetWorldLocation(const Vector2& newLocation)
{
    std::shared_ptr<USceneComponent> parent = attachParent.lock();
    if (!parent)
    {
        relativeLocation = newLocation;
        return;
    }
    
    relativeLocation = newLocation - parent->GetComponentLocation();
}