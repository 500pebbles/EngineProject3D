#pragma once
#include <World/World.h>

#include "Actor/CubeActor.h"
#include "Actor/PlayerActor.h"

class TestWorld : public UWorld
{
    TYPE_DECLARATIONS(TestWorld, UWorld)

public:
    void OnInitialized() override;
    
private:
    std::shared_ptr<PlayerActor> playerActor;   
    std::shared_ptr<CubeActor> cube1;
    std::shared_ptr<CubeActor> cube2;
    std::shared_ptr<CubeActor> cube3;
};
