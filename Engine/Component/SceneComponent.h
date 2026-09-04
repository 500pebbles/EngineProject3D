#pragma once
#include "ActorComponent.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"

class ENGINE_API USceneComponent : public UActorComponent
{
    TYPE_DECLARATIONS(USceneComponent, UActorComponent)

public:
    USceneComponent() = default;
    USceneComponent(const Vector3& relativeLocation);
    virtual ~USceneComponent() = default;
    
public:
    const Vector3& GetRelativeLocation() const { return relativeLocation; }
    const Vector3& GetRelativeRotation() const { return relativeRotation; }
    const Vector3& GetRelativeScale() const { return relativeScale; }
    
    void SetRelativeLocation(const Vector3& location) { relativeLocation = location; }
    void SetRelativeRotation(const Vector3& rotation) { relativeRotation = rotation; }
    void SetRelativeScale(const Vector3& scale) { relativeScale = scale; }

    Matrix4 GetLocalMatrix() const;
    Matrix4 GetWorldMatrix() const;
    
    Vector3 GetWorldLocation() const;
    void SetWorldLocation(const Vector3& location);    
    
public:
    /* 이 컴포넌트를 소유한 액터의 부모 액터 */
    std::shared_ptr<USceneComponent> GetAttachParent() const { return attachParent.lock(); }
    void SetAttachParent(std::weak_ptr<USceneComponent> newParent) { attachParent = newParent; }
    
protected:
    Vector3 relativeLocation { 0.0f, 0.0f, 0.0f };
    Vector3 relativeRotation { 0.0f, 0.0f, 0.0f };
    Vector3 relativeScale    { 1.0f, 1.0f, 1.0f };

    std::weak_ptr<USceneComponent> attachParent;
};
