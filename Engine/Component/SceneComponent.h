#pragma once
#include "ActorComponent.h"
#include "Math/Vector2.h"

class ENGINE_API USceneComponent : public UActorComponent
{
    TYPE_DECLARATIONS(USceneComponent, UActorComponent)

public:
    USceneComponent(const Vector2& relativeLocation = Vector2::Zero);
    virtual ~USceneComponent() = default;
    
    /* 컴포넌트의 월드위치 */
    Vector2 GetComponentLocation() const;
    void SetWorldLocation(const Vector2& newLocation);
    
    /* 컴포넌트의 부모기준 상대위치 */
    Vector2 GetRelativeLocation() const { return relativeLocation; }
    void SetRelativeLocation(const Vector2& newLocation) { relativeLocation = newLocation; }
    
    /* 이전 프레임의 컴포넌트 월드위치 */
    Vector2 GetPreviousComponentLocation() const { return previousComponentLocation; }
    void SetPreviousComponentLocation(const Vector2& newLocation) { previousComponentLocation = newLocation; }
    
    /* 컴포넌트를 소유한 액터가 부착된 액터 */
    inline std::shared_ptr<USceneComponent> GetAttachParent() const { return attachParent.lock(); }
    inline void SetParent(std::weak_ptr<USceneComponent> newParent) { attachParent = newParent; }
    
protected:
    Vector2 relativeLocation;
    Vector2 previousComponentLocation;
    
    std::weak_ptr<USceneComponent> attachParent;
};