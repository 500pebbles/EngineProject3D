#pragma once

#include <Core/Core.h>
#include <Math/Vector3.h>
#include <Math/Matrix4.h>
#include <Core/Object.h>
#include <memory>	
#include <vector>
#include "Component/ActorComponent.h"

class USceneComponent;

class ENGINE_API AActor : public UObject, public std::enable_shared_from_this<AActor>
{
	TYPE_DECLARATIONS(AActor, UObject)
		
	friend class ULevel;

public:
	AActor(const Vector3& location = Vector3(0.f, 0.f, 0.f));		
	virtual ~AActor();

public:
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Draw();
	virtual void OnCollision(const std::shared_ptr<AActor>& other);

	virtual void Destroy();
	void QuitGame();
	
	bool HasBeganPlay() const { return hasBeganPlay; }
	bool IsActive() const { return isActive && !hasExpired; }
	bool HasExpired() const { return hasExpired; }			
	
public:
	std::shared_ptr<ULevel> GetLevel() const { return level.lock(); }			
	void SetLevel(std::weak_ptr<ULevel> newLevel);
				
public:
	Vector3 GetActorLocation() const;
	void SetActorLocation(const Vector3& newLocation);
	Matrix4 GetActorWorldMatrix() const;
	
public:
	void AttachToActor(const std::shared_ptr<AActor>& newParent);	
	void DetachFromActor();
		
public:
	std::shared_ptr<AActor> GetAttachParentActor() const { return attachParentActor.lock(); }	
	const std::vector<std::weak_ptr<AActor>>& GetAttachedActors() const { return attachedActors; }	
	std::shared_ptr<USceneComponent> GetRootComponent() const{ return rootComponent; }
	
	void SetRootComponent(const std::shared_ptr<USceneComponent>& newRootComponent);

protected:
	void ProcessAddComponents();	
	void BindComponentOwners();
	
	
	
protected:
	bool hasBeganPlay = false;
	bool isActive = true;
	bool hasExpired = false;
	
protected:
	std::weak_ptr<ULevel> level;	
	
protected:		
	std::vector<std::shared_ptr<UActorComponent>> ownedComponents;	
	std::vector<std::shared_ptr<UActorComponent>> requestComponents;
	
	std::weak_ptr<AActor> attachParentActor;	
	std::vector<std::weak_ptr<AActor>> attachedActors;				
			
	std::shared_ptr<USceneComponent> rootComponent;			
	
protected:	
	Vector3 initialLocation = Vector3::Zero;
		
	
	
	
/* 컴포넌트 생성 & 검색 Template */	
public:		
	template<typename T, typename... Args,
	typename = std::enable_if_t<std::is_base_of<UActorComponent, T>::value>>
	std::shared_ptr<T> AddComponent(Args&&... args)
	{
		// 새로운 컴포넌트 생성후 추가요청 목록에 등록
		std::shared_ptr<T> newComponent = std::make_shared<T>(std::forward<Args>(args)...);
		requestComponents.emplace_back(newComponent);	
		
		// 생성한 컴포넌트 반환
		return newComponent;			
	}
	
	template<typename T, 
	typename = std::enable_if_t<std::is_base_of<UActorComponent, T>::value>>
	std::shared_ptr<T> GetComponent() const
	{
		// 컴포넌트 목록과 요청 목록을 전부 순회해 컴포넌트 T를 검색하고 반환
		for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
		{
			if (component && component->IsTypeOf<T>()) return std::static_pointer_cast<T>(component);
		}
		for (const std::shared_ptr<UActorComponent>& component : requestComponents)
		{
			if (component && component->IsTypeOf<T>()) return std::static_pointer_cast<T>(component);
		}

		// 두 리스트에 모두 없다면 return null 
		return nullptr;
	}
};	