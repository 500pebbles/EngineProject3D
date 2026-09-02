#pragma once
#include <vector>

#include "Level.h"
#include "Actor/Actor.h"
#include "Core/Object.h"

class ENGINE_API UWorld : public UObject
{
    TYPE_DECLARATIONS(UWorld, UObject)
    
    friend class Engine;
    
public:
    UWorld();
    virtual ~UWorld();

public:
    virtual void OnInitialized();
    virtual void BeginPlay();
    virtual void Tick(float deltaTime);
    virtual void Draw();		    
    
public:
    /* 월드에 액터 추가 (레벨 지정)  */
    template<
        typename T, 
        typename ...Args,
        typename = std::enable_if_t<std::is_base_of<AActor, T>::value>>
        std::shared_ptr<T> SpawnActor(Args&& ...args)
    {
        std::shared_ptr<T> newActor = std::make_shared<T>(std::forward<Args>(args)...);
        persistentLevel->requestedActors.emplace_back(newActor);
        newActor->SetLevel(persistentLevel);

        return newActor;  
    }

    /* 레벨에 존재하는 액터 검색 */
    template<
        typename T,
        typename = std::enable_if_t<std::is_base_of<AActor, T>::value>>
        std::shared_ptr<T> FindActor()
    {
        if (!persistentLevel) return nullptr;  
        
        for (const auto& actor : persistentLevel->actors)
        {
            std::shared_ptr<T> targetActor = std::dynamic_pointer_cast<T>(actor);
            if (targetActor) return targetActor;
        }
			
        return nullptr; 
    }
    
    
public:
    inline bool HasInitialized() const { return hasInitialized; }
    
protected:
    void ProcessAddAndDestroyActors();

    void SavePreviousActorStates();	

protected:    
    std::shared_ptr<ULevel> persistentLevel; 
    
    std::vector<std::shared_ptr<ULevel>> levels;

    bool hasInitialized = false;	       
};