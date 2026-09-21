#pragma once

#include <World/World.h>

#include "Actor/Player.h"
#include "Actor/Terrain.h"

class World03 : public UWorld
{
    TYPE_DECLARATIONS(World03, UWorld)    

public:    
    void OnInitialized() override;
    
    void Tick(float deltaTime) override;
private:
    std::shared_ptr<Player> player;
    
    std::shared_ptr<Terrain> terrain01;
    std::shared_ptr<Terrain> terrain02; 
    std::shared_ptr<Terrain> terrain03;
};
