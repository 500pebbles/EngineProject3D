#pragma once
#include "SceneComponent.h"

class UCameraComponent : public USceneComponent
{
    TYPE_DECLARATIONS(UCameraComponent, USceneComponent)
public:
    UCameraComponent() = default;
     ~UCameraComponent() = default;

public:
    Matrix4 GetViewMatrix() const;         
    
    const Vector3& GetTarget() const { return target; }
    void SetTarget(const Vector3& newTarget) { target = newTarget; }

    void SetFieldOfView(float value) { fieldOfView = value; }    
    const float GetFieldOfView() { return fieldOfView; }
    
    void SetNearPlane(float value) { nearPlane = value; }
    const float GetNearPlane() { return nearPlane; }  
    
    void SetFarPlane(float value) { farPlane = value; }
    const float GetFarPlane() { return farPlane ; }

private:
    Vector3 target = Vector3::Zero;
    
    float fieldOfView = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
};