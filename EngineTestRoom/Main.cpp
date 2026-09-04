
#include <Engine/Engine.h>

#include "World/TestWorld.h"


int main()
{
    Engine engine;
    engine.AddNewWorld<TestWorld>();
    
    
    engine.Run();    
    
}