#include "TestWorld.h"

#include "Actor/TestActor.h"
#include "Actor/WallActor.h"

void TestWorld::OnInitialized()
{
    
    UWorld::OnInitialized();
    
    SpawnActor<TestActor>(Vector2(20.f, 5.f));
    SpawnActor<WallActor>(Vector2(20.f, 10.f));
    
}
