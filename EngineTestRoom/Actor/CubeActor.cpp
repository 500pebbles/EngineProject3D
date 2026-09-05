#include "CubeActor.h"

#include <Component/MeshRendererComponent.h>
#include <Component/SceneComponent.h>   

CubeActor::CubeActor(const Vector3& location)
: AActor(location)
{
    meshRendererComponent = AddComponent<UMeshRendererComponent>();
    
    SetRootComponent(meshRendererComponent);
}

void CubeActor::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);    
    
    //SetActorRotation(GetActorRotation() + Vector3(0,0,1));
}
