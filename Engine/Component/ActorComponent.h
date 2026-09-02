#pragma once
#include "Core/Object.h"

class AActor;
    
class ENGINE_API UActorComponent : public UObject
{
    TYPE_DECLARATIONS(UActorComponent, UObject)

public:
    UActorComponent();
    virtual ~UActorComponent();
        
    virtual void BeginPlay();
    virtual void Tick(float DeltaTime);
    virtual void Draw();
        
    virtual void OnCollision(const std::shared_ptr<AActor>& other);    
    inline bool HasBeganPlay() const { return hasBeganPlay; }
    
    inline std::shared_ptr<AActor> GetOwner() const { return owner.lock(); }
    inline void SetOwner(std::weak_ptr<AActor> newOwner) { owner = newOwner; }
    
protected:
    bool hasBeganPlay = false;
        
    std::weak_ptr<AActor> owner;
};