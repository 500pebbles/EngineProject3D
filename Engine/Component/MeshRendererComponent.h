#pragma once
#include <Core/Core.h>
#include "PrimitiveComponent.h"
#include "Shape/Mesh.h"

class ENGINE_API UMeshRendererComponent : public UPrimitiveComponent
{
public:
    TYPE_DECLARATIONS(UMeshRendererComponent, UPrimitiveComponent)

public:
    UMeshRendererComponent();
    virtual ~UMeshRendererComponent() = default;

public:
    virtual void Draw() override;

    const Mesh& GetMesh() const { return mesh; }

    void SetMesh(const Mesh& newMesh) { mesh = newMesh; }

private:
    Mesh mesh;
};
