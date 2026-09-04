#pragma once
#include <vector>

#include "Level.h"
#include "Actor/Actor.h"
#include "Core/Object.h"

class UCameraComponent;

class ENGINE_API UWorld : public UObject, public std::enable_shared_from_this<UWorld>
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
    void SetActiveCamera(const std::shared_ptr<UCameraComponent>& camera);
    std::shared_ptr<UCameraComponent> GetActiveCamera() const;
    
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
    
protected:    
    std::shared_ptr<ULevel> persistentLevel; 
    
    std::vector<std::shared_ptr<ULevel>> levels;

    bool hasInitialized = false;	       
    
private:
    std::shared_ptr<UCameraComponent> activeCamera;
};

/*
 * 
강사님 요청드리고 싶은 사항이 두가지가 있습니다
1. Part2 프로젝트 개발 로그 시트에서 '이한설' 시트 삭제
사유 : 
아침에 일어나보니 폰이 고장나서 서브폰으로 구글로그인 인증절차를 했더니 기존 계정에 로그인을 못 하게됨.
다른계정으로 새로 시트를 만들 예정입니다.

2. 지난기수 프로젝트 시연영상을 볼 수 있다면 보고싶습니다
정확히는 1차프로젝트 직후에 보여주신 프로젝트중 구글가셨다는 분이 만든 '터미널 화면에 3D오브젝트를 띄운 프로젝트' 시연영상을 다시 보고싶습니다.

두번째는 사실 꼭 필요하지는 않습니다. 
 * 
 * 
 */

