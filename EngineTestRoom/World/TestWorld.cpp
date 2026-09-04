#include "TestWorld.h"

#include "Actor/CubeActor.h"

void TestWorld::OnInitialized()
{    
    UWorld::OnInitialized();
    
    SpawnActor<PlayerActor>(Vector3(3, 3, 3));
    
    cube1 = SpawnActor<CubeActor>(Vector3::Zero);   
    cube2 = SpawnActor<CubeActor>(Vector3(1,-1,3));
    cube3 = SpawnActor<CubeActor>(Vector3(-2,0.5,-1));
    
    cube2->AttachToActor(cube1);
    cube3->AttachToActor(cube2);
}
