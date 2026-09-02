#pragma once

#include <vector>
#include <memory>

class AActor;

class CollisionSystem
{
	struct CollisionPair  
	{
		std::shared_ptr<AActor> actor;
		std::shared_ptr<AActor> other;
	};

public:
	CollisionSystem() = default;
	~CollisionSystem() = default;

	void ProcessCollision(const std::vector<std::shared_ptr<AActor>>& actorList);

private:
	bool Test(const std::shared_ptr<AActor>& left, const std::shared_ptr<AActor>& right);
};