#include "Actor.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>

#include "Component/SceneComponent.h"


AActor::AActor(const Vector3& location) : initialLocation(location)
{
	
}	
	
AActor::~AActor()
{
	
}
	
void AActor::BeginPlay()
{
	hasBeganPlay = true;
		
	for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
	{
		if (!component->HasBeganPlay()) component->BeginPlay();
	}
}	

void AActor::Tick(float deltaTime)
{
	if (!IsActive()) return;	
		
	for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
	{
		component->Tick(deltaTime);
	}
}	

void AActor::Draw()
{
	if (!IsActive()) return;		
		
	for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
	{
		component->Draw();  
	}
}	

void AActor::OnCollision(const std::shared_ptr<AActor>& other)
{
	/* 엔진의 Run중 도중 collisionSystem->ProcessCollision 으로 호출
	 * 엔진은 메인 레벨의 모든 액터를 상대로, 현재위치를 체크 후 겹쳐있는 액터를 전부 저장함
	 * 그 후 저장된 액터의 OnCollision을 양쪽 모두에게 호출
	 * 세부 내용은 해당 액터에서, 상태 액터의 타입을 검사해 타입별로 다른 행동문 작성  
	 * 주의 : 한쪽이 다른쪽을 Destroy()하는 등 이미 처리한다면, 다른쪽에서는 행동 지정 x */
		
	if (!IsActive()) return;
		
	for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
	{
		component->OnCollision(other);
	}		
}

void AActor::Destroy()
{
	// 삭제 플래그 : 레벨에서 검사하고 다음프레임 전에 액터 제거
	hasExpired = true;
		
	// 자식 Actor가 있다면 자신 액터들도 함께 제거
	for (const std::weak_ptr<AActor>& child : attachedActors)
	{
		std::shared_ptr<AActor> childActor = child.lock();
		if (childActor) childActor->Destroy();
	}
}	

void AActor::QuitGame()
{
	Engine::Get().Quit();
}


void AActor::AttachToActor(const std::shared_ptr<AActor>& newParent)
{
	if (!newParent || newParent.get() == this || !rootComponent) return;
	
	DetachFromActor();
		
	attachParentActor = newParent;
	newParent->attachedActors.emplace_back(weak_from_this());
	
	const std::shared_ptr<USceneComponent> parentRoot = newParent->GetRootComponent();
	if (!parentRoot) return;
	
	rootComponent->SetAttachParent(parentRoot);
}
	
void AActor::DetachFromActor()
{
	std::shared_ptr<AActor> oldParent =	attachParentActor.lock();

	if (oldParent)
	{
		auto& children = oldParent->attachedActors;

		for (auto iterator = children.begin(); iterator != children.end(); ++iterator)
		{
			std::shared_ptr<AActor> actor = iterator->lock();

			if (actor.get() == this)
			{
				children.erase(iterator);
				break;
			}
		}
	}

	attachParentActor.reset();

	if (!rootComponent) return;
	
	if (rootComponent) rootComponent->SetAttachParent(std::weak_ptr<USceneComponent>());
}

void AActor::SetLevel(std::weak_ptr<ULevel> newLevel)
{
	level = newLevel;
		
	// 보유한 Component들의 Owner를 자신으로 설정  TODO : 적합한 위치는 아닌듯
	BindComponentOwners();	
}

Vector3 AActor::GetActorLocation() const
{
	if (!rootComponent) return Vector3::Zero;

	return rootComponent->GetWorldLocation();
}

void AActor::SetActorLocation(const Vector3& newLocation)
{
	if (!rootComponent) return;
	if (GetActorLocation() == newLocation) return;

	rootComponent->SetWorldLocation(newLocation);
}

Matrix4 AActor::GetActorWorldMatrix() const
{
	if (!rootComponent)	return Matrix4::Identity;

	return rootComponent->GetWorldMatrix();
}

void AActor::SetRootComponent(const std::shared_ptr<USceneComponent>& newRootComponent)
{
	rootComponent = newRootComponent;
	if (rootComponent) rootComponent->SetRelativeLocation(initialLocation);
}

void AActor::ProcessAddComponents()
{
	if (requestComponents.empty()) return;
		
	BindComponentOwners(); // 컴포넌트의 Owner를 현재 액터로 설정
		
	for (const std::shared_ptr<UActorComponent>& component : requestComponents)
	{
		if (!component) continue;
			
		// 요청목록을 실제 컴포넌트 목록이 추가
		ownedComponents.emplace_back(component);  
			
		// Actor가 이미 BeginPlay를 처리했다면 추가된 컴포넌트에서도 BeginPlay 호출
		if (hasBeganPlay && !component->HasBeganPlay())
		{
			component->BeginPlay();
		}
	}
		
	// 추가 완료 후 요청목록 초기화
	requestComponents.clear();
}

void AActor::BindComponentOwners()
{
	// actor는 현재 객체(자신)의 포인터 
	std::shared_ptr<AActor> actor = shared_from_this();
	if (!actor) return;
		
	// 컴포넌트 목록, 요청 목록 위치가 다를뿐 둘다 가리키는 대상은 실존하는 컴포넌트이므로
	for (const std::shared_ptr<UActorComponent>& component : ownedComponents)
	{
		if (component) component->SetOwner(actor);
	}
		
	// 새로운 컴포넌트가 있으면(!addRequestedComponentList.empty()) 두 목록을 가리지 않고 오너십 설정
	for (const std::shared_ptr<UActorComponent>& component : requestComponents)
	{
		if (component) component->SetOwner(actor);
	}
}	