#pragma once

#include <Core/Core.h>
#include <Actor/Actor.h>
#include <Core/Object.h>
#include <memory>	
#include <vector>	

class ENGINE_API ULevel : public UObject, public std::enable_shared_from_this<ULevel>
{
	TYPE_DECLARATIONS(ULevel, UObject)

	friend class UWorld;

public:
	ULevel(); 
	virtual ~ULevel();		

public:
	virtual void OnInitialized();
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Draw();		
		
public:
	inline bool HasInitialized() const { return hasInitialized; }

protected:
	void ProcessAddAndDestroyActors();
	
public:
	std::vector<std::shared_ptr<AActor>> actors;

	std::vector<std::shared_ptr<AActor>> requestedActors;		

protected:
	bool hasInitialized = false;
		
};