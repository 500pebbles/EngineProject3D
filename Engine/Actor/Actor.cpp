#include "Actor.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>

#include "Component/SceneComponent.h"


AActor::AActor(const Vector2& location) : initialLocation(location)
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
		// 다른 컴포넌트의 Draw()는 비워져있지만 SpriteComponent는 렌더링 정보값들을 Submit
		component->Draw();  
	}
}	

void AActor::OnCollision(const std::shared_ptr<AActor>& other)
{
	/* 엔진의 Run중 도중 collisionSystem->ProcessCollision 으로 호출
	 * 엔진은 메인 레벨의 모든 액터를 상대로, 현재&이전 위치를 체크 후 최악을 상정해 겹쳐있는 액터를 전부 저장함
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


void AActor::AttachToActor(const std::shared_ptr<AActor>& newParent, bool keepWorldLocation)
{
	if (!newParent || newParent.get() == this) return;
		
	DetachFromActor();
		
	attachParentActor = newParent;
	newParent->attachedActors.emplace_back(weak_from_this());
		
	if (rootComponent && newParent->GetRootComponent())
	{
		Vector2 worldLocation = rootComponent->GetComponentLocation();
		rootComponent->SetParent(newParent->GetRootComponent());

		if (keepWorldLocation) rootComponent->SetWorldLocation(worldLocation); 
	}
}
	
void AActor::DetachFromActor()
{
	std::shared_ptr<AActor> oldParent = attachParentActor.lock();

	// 기존 부모의 자식목록에서 this를 제거
	if (oldParent)
	{
		// 부모의 모든 자손목록을 대상으로
		auto& siblingList = oldParent->attachedActors;
		for (auto iterator = siblingList.begin(); iterator != siblingList.end(); ++iterator)
		{
			// 자손목록중 자신을 발견하면 제거하고 루프 탈출
			if ((*iterator).lock().get() == this)
			{
				siblingList.erase(iterator);
				break;
			}
		}
	}
		
	// 제거후 기존 부모참조 변수 초기화
	attachParentActor.reset();
		
	if (rootComponent)
    {
        Vector2 worldLocation = rootComponent->GetComponentLocation();
        rootComponent->SetParent(std::weak_ptr<USceneComponent>());
        rootComponent->SetWorldLocation(worldLocation);
    }
}

void AActor::SetLevel(std::weak_ptr<ULevel> newLevel)
{
	level = newLevel;
		
	// 보유한 Component들의 Owner를 자신으로 설정  TODO : 적합한 위치는 아닌듯
	BindComponentOwners();	
}

Vector2 AActor::GetActorLocation() const
{
	return rootComponent ? rootComponent->GetComponentLocation() : Vector2::Zero;
}

void AActor::SetActorLocation(const Vector2& newLocation)
{
	if (GetActorLocation() == newLocation) return;
		
	if (rootComponent) rootComponent->SetWorldLocation(newLocation);
}	

Vector2 AActor::GetPreviousActorLocation() const
{
	return rootComponent ? rootComponent->GetPreviousComponentLocation() : Vector2::Zero;
}	

void AActor::SavePreviousActorLocation()
{
	if (!rootComponent) return;
	rootComponent->SetPreviousComponentLocation(rootComponent->GetComponentLocation());
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