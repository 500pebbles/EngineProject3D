
#include <Engine/Engine.h>

#include "World/World01.h"
#include "World/World02.h"
#include "World/World03.h"


int main()
{
    Engine engine;
    engine.AddNewWorld<World01>();
    engine.Run();        
}