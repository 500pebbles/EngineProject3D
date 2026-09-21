#include "Terrain.h" 

#include <Component/MeshRendererComponent.h>
#include <Component/SphereComponent.h>   

Terrain::Terrain(const Vector3& location)
    : super(location)
{
    meshRendererComponent = AddComponent<UMeshRendererComponent>();        
    SetRootComponent(meshRendererComponent);
}

Terrain::~Terrain()
{
    
}

void Terrain::Tick(float deltaTime)
{
    AActor::Tick(deltaTime);        
    //SetActorRotation(GetActorRotation() + Vector3(-1.f,-1.f,0.f));
}