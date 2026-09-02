#include "CollisionSystem.h"
#include <Actor/Actor.h>
#include <Component/BoxComponent.h>

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
	if (!left || !right) return false;
	
	// 두 액터의 충돌 컴포넌트 확인
	std::shared_ptr<UBoxComponent> leftCollision = left->GetComponent<UBoxComponent>();
	std::shared_ptr<UBoxComponent> rightCollision = right->GetComponent<UBoxComponent>();
	if (!leftCollision || !rightCollision) return false;
					
	// left 액터 충돌박스의 현재/이전 위치
	const Vector2 leftCurrent = leftCollision->GetComponentLocation();
	const Vector2 leftPrevious = leftCollision->GetPreviousComponentLocation();

	// right 액터 충돌박스의 현재/이전 위치
	const Vector2 rightCurrent = rightCollision->GetComponentLocation();
	const Vector2 rightPrevious = rightCollision->GetPreviousComponentLocation();

	// 이전 프레임 위치와 현재 위치를 모두 포함하는 swept bounds 계산. (최악을 상정한 실제보다 긴 히트박스)
	const float leftXMin = (leftCurrent.x < leftPrevious.x) ? leftCurrent.x : leftPrevious.x;
	const float leftXMaxCurrent = leftCurrent.x + leftCollision->GetBoxWidth() - 1;
	const float leftXMaxPrevious = leftPrevious.x + leftCollision->GetBoxWidth() - 1;
	const float leftXMax = (leftXMaxCurrent > leftXMaxPrevious) ? leftXMaxCurrent : leftXMaxPrevious;

	const float rightXMin = (rightCurrent.x < rightPrevious.x) ? rightCurrent.x : rightPrevious.x;
	const float rightXMaxCurrent = rightCurrent.x + rightCollision->GetBoxWidth() - 1;
	const float rightXMaxPrevious = rightPrevious.x + rightCollision->GetBoxWidth() - 1;
	const float rightXMax = (rightXMaxCurrent > rightXMaxPrevious) ? rightXMaxCurrent : rightXMaxPrevious;

	// X좌표 기준으로 충돌이 발생할 수 없는 상황 처리.
	if (static_cast<int>(rightXMin) > static_cast<int>(leftXMax)) return false;    // 2번액터의 x좌표 최소치가 1번액터의 x좌표 최대치보다 오른쪽 = 떨어짐
	if (static_cast<int>(rightXMax) < static_cast<int>(leftXMin)) return false;    // 대충 비슷함
	
	
	const float leftYMin = (leftCurrent.y < leftPrevious.y) ? leftCurrent.y : leftPrevious.y;
	const float leftYMaxCurrent = leftCurrent.y + leftCollision->GetBoxHeight() - 1.f;
	const float leftYMaxPrevious = leftPrevious.y + leftCollision->GetBoxHeight() - 1.f;
	const float leftYMax = (leftYMaxCurrent > leftYMaxPrevious) ? leftYMaxCurrent : leftYMaxPrevious;

	const float rightYMin = (rightCurrent.y < rightPrevious.y) ? rightCurrent.y : rightPrevious.y;
	const float rightYMaxCurrent = rightCurrent.y + rightCollision->GetBoxHeight() - 1.f;
	const float rightYMaxPrevious = rightPrevious.y + rightCollision->GetBoxHeight() - 1.f;
	const float rightYMax = (rightYMaxCurrent > rightYMaxPrevious) ? rightYMaxCurrent : rightYMaxPrevious;
	
	
	// y좌표 기준으로 충돌이 발생할 수 없는 상황 처리.
	if (static_cast<int>(rightYMin) > static_cast<int>(leftYMax)) return false;
	if (static_cast<int>(rightYMax) < static_cast<int>(leftYMin)) return false;

	// 충돌 발생
	return true;
}