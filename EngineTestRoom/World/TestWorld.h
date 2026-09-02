#pragma once
#include <World/World.h>

class TestWorld : public UWorld
{
    TYPE_DECLARATIONS(TestWorld, UWorld)

public:
    void OnInitialized() override;
    
};
