#include "CollisionSystem.h"
#include <Actor/Actor.h>
#include <Component/BoxComponent.h>

#include "Component/SphereComponent.h"

void CollisionSystem::ProcessCollision(const std::vector<std::shared_ptr<AActor>>& actorList)
{
	// 레벨의 actorList가 비어있다면 충돌처리 x (actorList는 Engine::Run에서 mainLevel->actorList로 의존성 주입)
	if (actorList.empty()) return;
	
	// 해당 프레임에 충돌한 액터들에게 이벤트를 한번에 전달하기 위한 배열
	std::vector<CollisionPair> collidedActorList;

	// 레벨에 배치된 액터 수
	const int count = static_cast<int>(actorList.size());

	// 레벨에 배치된 모든 액터를 대상으로 충돌 검사
	for (int ix = 0; ix < count; ++ix)
	{
		const std::shared_ptr<AActor>& left = actorList[ix];
		if (!left || !left->IsActive()) continue;

		for (int jx = ix + 1; jx < count; ++jx)
		{
			const std::shared_ptr<AActor>& right = actorList[jx];
			if (!right || !right->IsActive()) continue;
			
			if (Test(left, right))   // 충돌 검사 실행 : Test()에서 두 액터의 위치를 교차검증 
			{
				// 충돌이 있었던 두 액터를 구조체에 담아 정리
				CollisionPair pair = {};
				pair.actor = left;
				pair.other = right;
				
				collidedActorList.emplace_back(pair); // 목록에 추가
			}
		}
	}

	// 충돌이 있었던 액터 목록이 비었다면 종료 
	if (collidedActorList.empty()) return;

	// 충돌한 액터 목록을 순회하며 이벤트 전달
	for (const CollisionPair& pair : collidedActorList)
	{
		// 이미 비활성화된 액터라면 스킵
		if (!pair.actor->IsActive() || !pair.other->IsActive()) continue;

		// 두 액터 모두에게 충돌 이벤트 전달
		pair.actor->OnCollision(pair.other);
		pair.other->OnCollision(pair.actor);
	}
}

bool CollisionSystem::Test(const std::shared_ptr<AActor>& left,	const std::shared_ptr<AActor>& right)
{
	/* 액터1, 액터2 스피어 null체크 */
	if (!left || !right) return false;
	const std::shared_ptr<USphereComponent> leftSphere = left->GetComponent<USphereComponent>();
	const std::shared_ptr<USphereComponent> rightSphere = right->GetComponent<USphereComponent>();
	if (!leftSphere || !rightSphere) return false;
    
	/* 액터1, 액터2 트랜스폼 Position */
	const Vector3 leftCenter = left->GetActorLocation();
	const Vector3 rightCenter = right->GetActorLocation();

	/* 두 액터의 위치차이를 벡터로 표현하고, 벡터 자신을 내적해서 길이값 확보 */
	const Vector3 distance = leftCenter - rightCenter;
	const float distanceLength = distance.Dot(distance);   // TODO: 얘는 sqrt하면 연산을 한번 더하는대신 코드가 깔끔할듯? 일단킵
	const float leftRightSphereRadiusSum = leftSphere->GetRadius() + rightSphere->GetRadius();

	/* 액터 사이의 length의 제곱과 두 액터 캡슐 반지름의 합의 제곱을 비교해 충돌판정 처리 */
	return distanceLength <= leftRightSphereRadiusSum * leftRightSphereRadiusSum;
}
