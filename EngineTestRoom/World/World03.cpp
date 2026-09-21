#include "World03.h"


void World03::OnInitialized()
{
    UWorld::OnInitialized();
    
    player = SpawnActor<Player>(Vector3(0, -10, 0));    
    
    terrain02->AttachToActor(terrain01);
    terrain03->AttachToActor(terrain02);
    
    SpawnActor<Terrain>(Vector3(15, 0, 0));
    SpawnActor<Terrain>(Vector3(15, 0, 2));
    SpawnActor<Terrain>(Vector3(15, 0, 4));
    SpawnActor<Terrain>(Vector3(15, 0, 6));
    SpawnActor<Terrain>(Vector3(15, 0, 8));
    SpawnActor<Terrain>(Vector3(15, 0, 10));
    SpawnActor<Terrain>(Vector3(15, 0, 12));
}

void World03::Tick(float deltaTime)
{
    UWorld::Tick(deltaTime);
    
    for (auto actor : GetActiveLevel()->actors)
    {
        if (actor->IsTypeOf<Terrain>())
        {
            actor->SetActorLocation(actor->GetActorLocation() - Vector3(0, 0.05f, 0));
        }
    }
}
