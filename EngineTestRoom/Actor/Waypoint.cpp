#include "Waypoint.h"
#include <Component/MeshRendererComponent.h>

Waypoint::Waypoint(const Vector3& location)
    : super(location)
{
    meshRendererComponent = AddComponent<UMeshRendererComponent>();   
    SetRootComponent(meshRendererComponent);
     
    meshRendererComponent->SetMeshColor(Color::BrightYellow);
    SetActorScale(Vector3(0.5f, 0.5f, 0.5f));
    
    meshRendererComponent->SetVisibility(false);
}

Waypoint::~Waypoint()
{
}


void Waypoint::SetMeshColor(Color color)
{
    meshRendererComponent->SetMeshColor(color); 
}

void Waypoint::SetVisibility(bool visible)
{
    meshRendererComponent->SetVisibility(visible);
}
