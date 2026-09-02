#include "ActorComponent.h"

UActorComponent::UActorComponent()
{
}

UActorComponent::~UActorComponent()
{
}

void UActorComponent::BeginPlay()
{
    hasBeganPlay = true;
}

void UActorComponent::Tick(float DeltaTime)
{
}

void UActorComponent::Draw()
{
}

void UActorComponent::OnCollision(const std::shared_ptr<AActor>& other)
{
}