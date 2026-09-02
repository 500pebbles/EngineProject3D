#include "SpriteRendererComponent.h"

#include <cmath>
#include <Actor/Actor.h>
#include <Render/Renderer.h>

USpriteRendererComponent::USpriteRendererComponent(const std::string& image, Color color, int sortingOrder)
   : image(image),color(color),sortingOrder(sortingOrder)
{
}

void USpriteRendererComponent::Draw()
{
    super::Draw();
        
    std::shared_ptr<AActor> actor = GetOwner();        
    if (!actor || !actor->IsActive()) return;
    
    RenderPosition renderPosition(
       static_cast<int>((actor->GetActorLocation().x)),
       static_cast<int>((actor->GetActorLocation().y))
   );
    
    Submit(renderPosition);    
}

void USpriteRendererComponent::Submit(const RenderPosition& position)
{
    Renderer::Get().Submit(image, position, color, sortingOrder);        
}