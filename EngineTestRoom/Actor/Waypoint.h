#pragma once
#include <Actor/Actor.h>
#include <Component/MeshRendererComponent.h>

#include "NavigationSystem/Position.h"

class UMeshRendererComponent;

class Waypoint : public AActor
{
    TYPE_DECLARATIONS(Waypoint, AActor)

public:
    Waypoint(const Vector3& location);
    ~Waypoint();
    
    void SetGridPosition(const Position& position) { gridPosition = position; }

    const Position& GetGridPosition() const { return gridPosition; }
    
    void SetMeshColor(Color color);
    
    void SetVisibility(bool visible);
    
    
private:
    std::shared_ptr<UMeshRendererComponent> meshRendererComponent;
    
    Position gridPosition;
};
