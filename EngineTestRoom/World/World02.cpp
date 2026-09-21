#include "World02.h"

#include <Input/Input.h>
#include <World/World.h>
#include <Actor/Player.h>
#include "Actor/Terrain.h"
#include "Actor/Stalker.h"
#include "Actor/Target.h"
#include "NavigationSystem/AStarAlgorithm.h"
#include "Actor/Waypoint.h"
#include <Util/Util.h>


void World02::OnInitialized()
{    
    UWorld::OnInitialized();        
    
    /* 카메라 생성 */
    player = SpawnActor<Player>(Vector3(0, 5, 0));    
    
    /* 액터목록을 담고있는 3단배열 그리드 생성  */ 
    CreateTerrains();    
    
    /* 그리드를 기반으로 액터와 웨이포인트 생성 */
    actorGrid = CreateActor();
    waypointGrid = CreateWaypoint(actorGrid);    
}

void World02::Tick(float deltaTime)
{
    UWorld::Tick(deltaTime);    
    
    ProcessInput();
    UpdateTargetRandomMove(deltaTime);
    UpdateAStarVisualization();    
    UpdateActorChase(deltaTime);
}

void World02::ProcessInput()
{
    Input& input = Input::Get();  
    if (input.GetKey(VK_RETURN)) StartActorChase();  
}

    void World02::UpdateAStarVisualization()
{          
    HideWaypoints();
    for (const Position& position : finalPathPositions)
    {
        std::shared_ptr<Waypoint> waypoint = waypointActors[position.z][position.y][position.x];
        
        if (!waypoint) continue;          
        waypoint->SetVisibility(true);
        waypoint->SetMeshColor(Color::Blue);              
    }
}


void World02::StartPathfindAlgorithm()
{        
    /* A-Star 알고리즘 실행 */
    Position startPosition;
    Position goalPosition;
    
    FindStartAndGoalPosition(waypointGrid, startPosition, goalPosition);       
    
    
    int startPositionX = static_cast<int>(stalker->GetActorLocation().x);
    int startPositionY = static_cast<int>(stalker->GetActorLocation().z); 
    int startPositionZ = static_cast<int>(stalker->GetActorLocation().y);
    int goalPositionX = static_cast<int>(target->GetActorLocation().x);
    int goalPositionY = static_cast<int>(target->GetActorLocation().z);
    int goalPositionZ = static_cast<int>(target->GetActorLocation().y);
    
    startPosition = Position(startPositionX, startPositionY, startPositionZ);
    goalPosition = Position(goalPositionX, goalPositionY, goalPositionZ);
    
    AStarAlgorithm aStar;        
    finalPathPositions = aStar.FindPath(startPosition, goalPosition, waypointGrid, actorGrid);    
    
    /* 알고리즘 실행결과로 나온 Waypoint 위치목록을 받아 하나씩 생성 */
    openWaypointPositions = aStar.GetOpenWaypointPosition();
    openWaypointIndex = 0;
    hasFinishedVisualization = false;
    openWaypointTimer.CheckStartStat();    
}

void World02::StartActorChase()
{
    if (!stalker) return;

    chasePathIndex = 0;
    isActorChasing = true;
}

std::vector<std::vector<std::vector<int>>> World02::CreateActor()
{
    /* 액터생성용 그리드 */
    std::vector<std::vector<std::vector<int>>> actorGrid = terrain01;
    
    for (int z = 0; z < static_cast<int>(actorGrid.size()); ++z)
    {
        for (int y = 0; y < static_cast<int>(actorGrid[z].size()); ++y)
        {
            for (int x = 0; x < static_cast<int>(actorGrid[z][y].size()); ++x)
            {
                if (actorGrid[z][y][x] == 0) continue;
                if (actorGrid[z][y][x] == 1) SpawnActor<Terrain>(Vector3{static_cast<float>(x),static_cast<float>(z),static_cast<float>(y)});               
                if (actorGrid[z][y][x] == 2) stalker = SpawnActor<Stalker>(Vector3{static_cast<float>(x),static_cast<float>(z),static_cast<float>(y)});
                if (actorGrid[z][y][x] == 3) target = SpawnActor<Target>(Vector3{static_cast<float>(x),static_cast<float>(z),static_cast<float>(y)});
            }
        }
    }
    
    return actorGrid;
}

std::vector<std::vector<std::vector<int>>> World02::CreateWaypoint(const std::vector<std::vector<std::vector<int>>>& actorGrid)
{
    /* 탐색 웨이포인트용 그리드 */
    std::vector<std::vector<std::vector<int>>> waypointGrid = actorGrid;
    for (auto& layer : waypointGrid) for (auto& row : layer)  std::fill(row.begin(), row.end(), 0);
    
    /* 시각화용 웨이포인트 액터도 똑같은 사이즈 */
    waypointActors.resize(actorGrid.size());
    for (int z = 0; z < static_cast<int>(actorGrid.size()); ++z)
    {
        waypointActors[z].resize(actorGrid[z].size());
        for (int y = 0; y < static_cast<int>(actorGrid[z].size()); ++y)
        {
            waypointActors[z][y].resize(actorGrid[z][y].size());
        }
    }
    
    for (int z = 0; z < static_cast<int>(actorGrid.size()-1); ++z)
    {
        for (int y = 0; y < static_cast<int>(actorGrid[z].size()); ++y)
        {
            for (int x = 0; x < static_cast<int>(actorGrid[z][y].size()); ++x)
            {
                /* 웨이포인트 = 1,  지나갈수 없음 = 0 */
                
                /* 지형이 있건 없건 첫번째 z축은 전부 웨이포인트가 아님 */
                if (z == 0) waypointGrid[z][y][x] = 0;
                
                /* Stalker와 Target 노드위치 설정 */
                if (actorGrid[z][y][x] == 2) waypointGrid[z][y][x] = 2;
                if (actorGrid[z][y][x] == 3) waypointGrid[z][y][x] = 3;
                
                /* 지형이 존재하고, 그 위에 다른 지형이 없다면 그 위치는 웨이포인트로 설정 */
                if (actorGrid[z][y][x] == 1 && actorGrid[z+1][y][x] == 0)
                {
                    waypointGrid[z+1][y][x] = 1;
                    std::shared_ptr<Waypoint> waypoint = SpawnActor<Waypoint>(Vector3(static_cast<float>(x),static_cast<float>(z + 1),static_cast<float>(y)));
                    waypoint->SetGridPosition(Position(x, y, z + 1));
                    waypointActors[z + 1][y][x] = waypoint;                    
                }
            }
        }
    }
    
    return waypointGrid;
}


bool World02::FindStartAndGoalPosition(const std::vector<std::vector<std::vector<int>>>& grid, Position& outStartPosition, Position& outGoalPosition)
{
    if (grid.empty() || grid[0].empty() || grid[0][0].empty()) return false;

    // 시작점과 목표점의 중복 여부를 확인한다.
    bool foundStart = false;
    bool foundGoal = false;

    for (int z = 0; z < static_cast<int>(grid.size()); ++z)
    {
        for (int y = 0; y < static_cast<int>(grid[z].size()); ++y)
        {
            for (int x = 0; x < static_cast<int>(grid[z][y].size()); ++x)
            {
                if (grid[z][y][x] == 2)
                {
                    // 시작 위치는 정확히 하나만 허용한다.
                    if (foundStart)
                    {
                        return false;
                    }

                    outStartPosition = Position(x, y, z);
                    foundStart = true;
                }
                else if (grid[z][y][x] == 3)
                {
                    // 목표 위치도 정확히 하나만 허용한다.
                    if (foundGoal)
                    {
                        return false;
                    }

                    outGoalPosition = Position(x, y, z);
                    foundGoal = true;
                }
            }
        }
    }
    

    return foundStart && foundGoal;
}

void World02::UpdateActorChase(float deltaTime)
{
    if (!isActorChasing) return;
    
    StartPathfindAlgorithm();
    
    if (chasePathIndex >= finalPathPositions.size())
    {
        isActorChasing = false;
        return;
    }    
    
    const Position& pathPosition = finalPathPositions[chasePathIndex];
    const Vector3 targetPosition(static_cast<float>(pathPosition.x),static_cast<float>(pathPosition.z),static_cast<float>(pathPosition.y));

    const Vector3 currentPosition = stalker->GetActorLocation();
    const Vector3 direction = targetPosition - currentPosition;
    const float distance = direction.Length();    
    
    if (distance < 0.05f)
    {
        stalker->SetActorLocation(targetPosition);
        ++chasePathIndex;        
        return;
    }

    const Vector3 moveDirection = direction.Normalized();        
    stalker->SetActorLocation(currentPosition + moveDirection * stalkerMoveSpeed * deltaTime);       
}

void World02::UpdateTargetRandomMove(float deltaTime)
{
    if (!target) return;

    const Vector3 destination(14.0f, 1.0f, 1.0f);
    const Vector3 currentPosition = target->GetActorLocation();
    const Vector3 direction = destination - currentPosition;
    const float moveDistance = targetMoveSpeed * deltaTime * 0.5f;

    if (direction.Length() <= moveDistance)
    {
        target->SetActorLocation(destination);
        return;
    }

    target->SetActorLocation(currentPosition + direction.Normalized() * moveDistance);
}

void World02::ShowWaypoints()
{
    for (auto waypoint : GetActiveLevel()->actors)
    {
        if (waypoint->IsTypeOf<Waypoint>())
        {
            std::shared_ptr<Waypoint> wp = Cast<Waypoint>(waypoint);
            wp->SetVisibility(true);
        }
    }
}

void World02::HideWaypoints()
{
    for (auto waypoint : GetActiveLevel()->actors)
    {
        if (waypoint->IsTypeOf<Waypoint>())
        {
            std::shared_ptr<Waypoint> wp = Cast<Waypoint>(waypoint);
            wp->SetVisibility(false);
        }
    }
}


void World02::CreateTerrains()
{
    terrain01 =
    {
           /* 1층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 },
            { 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            } ,
          /* 2층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
            { 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 3, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
           } ,
           /* 3층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
           } ,
           /* 4층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            }                
    };            
    
    terrain02 =
    {
           /* 1층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 },
            { 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
            { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0 },
            { 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            } ,
          /* 2층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0 },
            { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0 },
            { 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 2, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
           } ,
           /* 3층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 3, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
           } ,
           /* 4층 */
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
            }                
    };            
    
    
    terrain03 =
    {
     /* 1층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 2층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 2, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 3층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 4층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 5층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    }
};
        
    
    terrain04 =
        {
    /* 1층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 2층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 3층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 4층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    },

    /* 5층 */
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    }
};
    
}
