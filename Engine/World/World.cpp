#include "World.h"

UWorld::UWorld()
{
    /* 현 구조 : World는 PersistentLevel 레벨 하나만을 보유 
     * 이후 움직이지 않는 고정된 Actor들은 Level단위로 묶어 스트리밍 처리 
     * SpawnActor에 파라미터로 레벨을 지정하도록 하고, 모든 액터 충돌처리는 World가 보유하는 actors 추가 */
    persistentLevel = std::make_shared<ULevel>();
    levels.push_back(persistentLevel);
}

UWorld::~UWorld()
{
}

void UWorld::OnInitialized()
{
    hasInitialized = true;
    for (auto level : levels)
    {
        level->OnInitialized();
    }
}

void UWorld::BeginPlay()
{
    for (auto level : levels)
    {
        level->BeginPlay();
    }
}

void UWorld::Tick(float deltaTime)
{
    for (auto level : levels)
    {
        level->Tick(deltaTime);
    }
}

void UWorld::Draw()
{
    for (auto level : levels)
    {
        level->Draw();
    }
}

void UWorld::ProcessAddAndDestroyActors()
{
    for (auto level : levels)
    {
        level->ProcessAddAndDestroyActors();
    }
}
