#pragma once
#include <World/World.h>
#include "Actor/Player.h"
#include "NavigationSystem/Position.h"
#include <Util/Timer.h>

class Waypoint;
class Position;
class Target;
class Stalker;

class World01 : public UWorld
{
    TYPE_DECLARATIONS(World01, UWorld)    

public:
    void OnInitialized() override;    
    void Tick(float deltaTime) override;    
    
private:
    void ProcessInput();
    void UpdateAStarVisualization();

    
private:
    std::vector<std::vector<std::vector<int>>> CreateActor();    
    std::vector<std::vector<std::vector<int>>> CreateWaypoint(const std::vector<std::vector<std::vector<int>>>& grid);
    bool FindStartAndGoalPosition(const std::vector<std::vector<std::vector<int>>>& grid, Position& outStartPosition, Position& outGoalPosition);

    void StartPathfindAlgorithm();
    void StartActorChase();
    void Reset();
    void ResetWaypoints();
    
private:
    std::shared_ptr<Player> player;       
    
private:    
    std::vector<std::vector<std::vector<std::shared_ptr<Waypoint>>>> waypointActors;
  
    std::vector<Position> openWaypointPositions;
    std::vector<Position> finalPathPositions;
    
    size_t openWaypointIndex = 0;
    StatTimer openWaypointTimer{};
    bool hasFinishedVisualization = false;    
    
    std::shared_ptr<Stalker> stalker;
    std::shared_ptr<Target> target;
    
/* Map Presets */    
private:
    std::vector<std::vector<std::vector<int>>> actorGrid;
    std::vector<std::vector<std::vector<int>>> waypointGrid;    
    
    std::vector<std::vector<std::vector<int>>> terrain01;
    std::vector<std::vector<std::vector<int>>> terrain02;
    std::vector<std::vector<std::vector<int>>> terrain03;
    std::vector<std::vector<std::vector<int>>> terrain04;
    std::vector<std::vector<std::vector<int>>> terrain05;
    
    void CreateTerrains();
    
/* Debug Section */
    void ShowWaypoints();
    void HideWaypoints();
    void PrintFinalPathCost();
    
    
/* Actor Chase System */
    void UpdateActorChase(float deltaTime);
    bool isActorChasing = false;
    size_t chasePathIndex = 0;
    float stalkerMoveSpeed = 3.0f;
    
};