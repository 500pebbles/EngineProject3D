#include "Level.h"


ULevel::ULevel()
{		
		
}

ULevel::~ULevel()
{
	
}

void ULevel::OnInitialized()
{
	hasInitialized = true;
}

void ULevel::BeginPlay()
{
	for (const std::shared_ptr<AActor>& actor : actors)
	{			
		if (actor->HasBeganPlay()) continue; 
		actor->BeginPlay();	
	}
}

void ULevel::Tick(float deltaTime)
{
	for (const std::shared_ptr<AActor>& actor : actors)
	{
		if (!actor->IsActive()) continue;
		actor->Tick(deltaTime);	
	}				
}

void ULevel::Draw()
{
	for (const std::shared_ptr<AActor>& actor : actors)
	{
		if (!actor->IsActive()) continue;
		actor->Draw();
	}
}

void ULevel::ProcessAddAndDestroyActors()
{
	/* Step 0 : 액터의 컴포넌트 추가 처리  */
	for (const std::shared_ptr<AActor>& actor : actors)
	{
		if (actor) actor->ProcessAddComponents();
	}
		
	/* Step 1 : 액터 제거 처리 */	
		
	for (auto iterator = actors.begin(); iterator != actors.end();)
	{
		// 제거 요청된 액터인지 확인 후 제거
		auto actor = *iterator;
		if (actor->HasExpired())
		{
			iterator = actors.erase(iterator); // 배열에서 빠져나가면 메모리도 해제 (shared_ptr)
			continue;
		}

		++iterator;
	}

	/* Step 2 : 액터 추가 처리 */
		
	// 추가요청 목록이 비어있으면 스킵
	if (requestedActors.empty()) return;   

	// 추가요청된 액터를 전부 actorList에 추가 
	for (const auto& actor : requestedActors) actors.emplace_back(actor);

	// 추가 완료되면 목록 정리.
	requestedActors.clear();
}
