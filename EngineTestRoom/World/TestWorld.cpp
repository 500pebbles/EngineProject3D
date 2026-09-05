#include "TestWorld.h"
#include <Render/Renderer3D.h>
#include "Actor/CubeActor.h"

void TestWorld::OnInitialized()
{    
    UWorld::OnInitialized();
    
    SpawnActor<PlayerActor>(Vector3(0, 0, 0));
    
    cube2 = SpawnActor<CubeActor>(Vector3(0, -10, 0));
    
    cube2->SetActorScale(Vector3(10,1,10));
    
    
    //cube1 = SpawnActor<CubeActor>(Vector3::Zero);   
}
