#include "MeshRendererComponent.h"

#include "Render/Renderer3D.h"

UMeshRendererComponent::UMeshRendererComponent()
{
    mesh = Mesh::CreateCube();
}

void UMeshRendererComponent::Draw()
{
    super::Draw();

    Renderer3D::Get().Submit(mesh,GetWorldMatrix());
}
