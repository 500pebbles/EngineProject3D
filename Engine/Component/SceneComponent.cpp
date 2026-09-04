#include "SceneComponent.h"

USceneComponent::USceneComponent(const Vector3& relativeLocation)
    : relativeLocation(relativeLocation)
{
}

Matrix4 USceneComponent::GetLocalMatrix() const
{
    /* SRT 행렬곱 반환 */
    const Matrix4 scaleMatrix = Matrix4::Scale(relativeScale);
    const Matrix4 rotationMatrix = Matrix4::Rotation(relativeRotation);
    const Matrix4 translationMatrix = Matrix4::Translation(relativeLocation);

    return scaleMatrix * rotationMatrix * translationMatrix;
}

Matrix4 USceneComponent::GetWorldMatrix() const
{
    /* 부모가 있다면 SRT x 부모SRT (재귀호출), 없다면 WorldMatrix = LocalMatrix */
    const Matrix4 localMatrix = GetLocalMatrix();    
    const std::shared_ptr<USceneComponent> parent = attachParent.lock();
    if (!parent) return localMatrix;

    return localMatrix * parent->GetWorldMatrix();
}

Vector3 USceneComponent::GetWorldLocation() const
{
    return Vector3::Zero * GetWorldMatrix();
}

void USceneComponent::SetWorldLocation(const Vector3& location)
{
    const std::shared_ptr<USceneComponent> parent = attachParent.lock();

    if (!parent)
    {
        relativeLocation = location;
        return;
    }

    const Matrix4 inverseParentWorld = Matrix4::Inverse(parent->GetWorldMatrix());
    relativeLocation = location * inverseParentWorld;
}
