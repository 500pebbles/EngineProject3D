#define NOMINMAX

#include "AStarAlgorithm.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <Windows.h>

#include "Math/Vector3.h"

AStarAlgorithm::AStarAlgorithm()
	: startNode(nullptr), goalNode(nullptr)
{
	
}

AStarAlgorithm::~AStarAlgorithm()
{
	Clear();
}

void AStarAlgorithm::Clear()
{
	/* 메모리 해제작업 */
	for (Node* node : allocatedNodes) delete node;

	allocatedNodes.clear();
	openList.clear();
	closedList.clear();

	startNode = nullptr;
	goalNode = nullptr;
		
	openWaypointPositions.clear();
}

Node* AStarAlgorithm::CreateNode(const Position& position, Node* parentNode)
{
	Node* node = new Node(position, parentNode);
	allocatedNodes.emplace_back(node);
	return node;
}

std::vector<Position> AStarAlgorithm::FindPath(
	const Position& startPosition,
	const Position& goalPosition,
	const std::vector<std::vector<std::vector<int>>>& waypointGrid,
	const std::vector<std::vector<std::vector<int>>>& actorGrid)
{
	Clear();

	// if (!IsValidGrid(grid)) return {};
	// if (!IsInRange(startPosition.x, startPosition.y, grid) || !IsInRange(goalPosition.x, goalPosition.y, grid)) return {}; 
	//if (grid[startPosition.y][startPosition.x] == 1 || grid[goalPosition.y][goalPosition.x] == 1) return {};
	
	/* 시작노드·목표노드 설정 */
	startNode = CreateNode(startPosition);
	goalNode = CreateNode(goalPosition);

	/* 최초 비용 초기화 */
	startNode->gCost = 0.0f;
	startNode->hCost = CalculateHeuristic(startPosition, goalPosition);
	startNode->fCost = startNode->gCost + startNode->hCost;
	
	/* openList는 발견했지만, 아직 조사하지 않은 후보노드 목록 */
	openList.emplace_back(startNode);

	/* 이동비용 설정 */
	Vector3 v0 = Vector3(0.f, 0.f, 0.f);  // 베이스   
	Vector3 v1 = Vector3(1.f, 0.f, 0.f);  // 앞
	Vector3 v2 = Vector3(1.f, 0.f, 1.f);  // 대각선
	Vector3 v3 = Vector3(1.f, 1.f, 0.f);  // 앞+한칸위 
	Vector3 v4 = Vector3(1.f, 1.f, 1.f);  // 대각+한칸위 
	
	const float straightCost = (v1 - v0).Length();
	const float diagonalCost = (v2 - v0).Length();
	const float upStraightCost = (v3 - v0).Length();
	const float upDiagonalCost = (v4 - v0).Length();
	
	/* 검사 가능한 24방향 설정 */
	const std::vector<Direction> directions =
	{
		/* z축좌표 이동량 : 0 */
		{ 0, -1, 0, straightCost  }, { 0, 1, 0, straightCost },
		{ -1, 0, 0, straightCost  }, { 1, 0, 0, straightCost },
		{ -1, -1, 0, diagonalCost }, {1, -1, 0, diagonalCost },
		{ -1, 1, 0, diagonalCost  }, { 1, 1, 0, diagonalCost },
		
		/* z축좌표 이동량 : 1 */
		{ 0, -1, 1, upStraightCost  }, { 0, 1, 1, upStraightCost },
		{ -1, 0, 1, upStraightCost  }, { 1, 0, 1, upStraightCost },
		{ -1, -1, 1, upDiagonalCost }, {1, -1, 1, upDiagonalCost },
		{ -1, 1, 1, upDiagonalCost  }, { 1, 1, 1, upDiagonalCost },
		
		/* z축좌표 이동량 : -1 */
		{ 0, -1, -1, upStraightCost  }, { 0, 1, -1, upStraightCost },
		{ -1, 0, -1, upStraightCost  }, { 1, 0, -1, upStraightCost },
		{ -1, -1, -1, upDiagonalCost }, {1, -1, -1, upDiagonalCost },
		{ -1, 1, -1, upDiagonalCost  }, { 1, 1, -1, upDiagonalCost }
	};

	/* 목적지를 찾을때까지 탐색 시작 */
	while (!openList.empty())
	{
		/* openList를 전부 검사, fCost가 가장 작은 노드를 currentNode로 설정 */
		Node* currentNode = openList[0];
		for (Node* node : openList)
		{
			if (node->fCost < currentNode->fCost || (node->fCost == currentNode->fCost && node->hCost < currentNode->hCost))  // fCost가 같다면 hCost가 적은쪽이 우선
			{
				currentNode = node;
			}
		}
		
		/* 현재 currentNode는 모든 openList중 가장 fCost가 낮은 노드 */
		
		/* COMPLETE : 목표에 도착했을 경우 최종경로만을 담은 배열반환 */
		if (IsDestination(currentNode)) return ConstructPath(currentNode);
		
		/* currentNode는 이제 탐색후보목록이 아닌, 이제부터 탐색을 시작하는 노드이므로 closedNode로 이동 */
		auto iterator = std::find(openList.begin(), openList.end(), currentNode);
		if (iterator != openList.end()) openList.erase(iterator);
		closedList.emplace_back(currentNode);

		/* currentNode가 이동할수있는 8방향의 노드를 체크한다 */
		for (const Direction& direction : directions)
		{
			/* 현재위치 + direction의 방향값을 더해 다음위치를 계산 */
			int newX = currentNode->position.x + direction.x;
			int newY = currentNode->position.y + direction.y;
			int newZ = currentNode->position.z + direction.z;

			/* 다음위치가 범위를 벗어나거나 */
			if (!IsInRange(newX, newY, newZ, waypointGrid)) continue;

			/* 웨이포인트가 아니라면 스킵 */
			if (waypointGrid[newZ][newY][newX] == 0) continue;

			/* 장애물이 있는데 대각선이동을 시도할경우 블락 */
			if (IsDiagonalBlocked(currentNode->position, direction, actorGrid)) continue;

			/* closeNode는 이동선택지에서 제외 */
			if (IsInClosedList(newX, newY, newZ)) continue;

			/* 여태까지 사용한 gCost + 다음이동까지 필요한 cost를 더해 새로운 gCost를 생성 */
			float newGCost = currentNode->gCost + direction.cost;

			/* 만약 다음 탐색하고자하는 노드가 이미 openNode목록에 있다면, 거기까지 가는데 걸리는 비용gCost가 더 싼쪽이 이제 정식 루트가 된다(parent = currentNode)  */
			Node* openNode = FindOpenNode(newX, newY, newZ);
			if (openNode != nullptr)
			{
				if (newGCost < openNode->gCost)
				{
					openNode->gCost = newGCost;
					openNode->fCost = openNode->gCost + openNode->hCost;
					openNode->parentNode = currentNode;
				}
				continue;
			}

			/* 다음위치가 openList에 없었다면 새로 노드를 생성한다 */
			Node* neighborNode = CreateNode(Position(newX, newY, newZ), currentNode);
			neighborNode->gCost = newGCost;
			neighborNode->hCost = CalculateHeuristic(neighborNode->position, goalNode->position);
			neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

			/* 새 노드를 openList에 추가한다 */
			openList.emplace_back(neighborNode);

			openWaypointPositions.emplace_back(neighborNode->position);		
		}
	}

	return {};
}

std::vector<Position> AStarAlgorithm::ConstructPath(Node* destinationNode)
{
	/* FindPath 알고리즘이 목적지에 도달했을때, 부모를 따라가며 '최종경로'만 남긴다 */
	std::vector<Position> path;
	Node* currentNode = destinationNode;

	while (currentNode != nullptr)
	{
		path.emplace_back(currentNode->position);
		currentNode = currentNode->parentNode;
	}

	std::reverse(path.begin(), path.end());
	return path;
}

float AStarAlgorithm::CalculateHeuristic(const Position& currentPosition, const Position& goalPosition) const
{
	/* 유클리드가 옥타일에 비해 부정확하기는 한데 현시점 구현가능한 유일한 휴리스틱 */
	const int dx = currentPosition.x - goalPosition.x;
	const int dy = currentPosition.y - goalPosition.y;
	const int dz = currentPosition.z - goalPosition.z;
	return std::sqrt(static_cast<float>(dx * dx + dy * dy + dz * dz));
}

bool AStarAlgorithm::IsValidGrid(const std::vector<std::vector<std::vector<int>>>& grid) const
{
	if (grid.empty() || grid[0].empty() || grid[0][0].empty()) return false;

	const size_t ySize = grid[0].size();
	const size_t xSize = grid[0][0].size();

	for (const std::vector<std::vector<int>>& layer : grid)
	{
		if (layer.size() != ySize) return false;

		for (const std::vector<int>& row : layer)
		{
			if (row.size() != xSize) return false;
		}
	}

	return true;
}

bool AStarAlgorithm::IsInRange(int x, int y, int z, const std::vector<std::vector<std::vector<int>>>& grid) const
{
	return x >= 0 && x < static_cast<int>(grid[0][0].size())
		&& y >= 0 && y < static_cast<int>(grid[0].size())
		&& z >= 0 && z < static_cast<int>(grid.size());
}

bool AStarAlgorithm::IsDiagonalBlocked(const Position& currentPosition,	const Direction& direction,	const std::vector<std::vector<std::vector<int>>>& actorGrid) const
{
	/* XY 기준 대각선 이동이 아니면 검사할 필요 없음 */
	if (direction.x == 0 || direction.y == 0) return false;

	const int desiredX = currentPosition.x + direction.x;
	const int desiredY = currentPosition.y + direction.y;
	const int desiredZ = currentPosition.z + direction.z;

	/* 평행이동/하강이동 = 현재층 검사, 상승이동 = 목적층 검사 */
	const int checkZ = std::max(currentPosition.z, desiredZ);

	const bool blockedX = actorGrid[checkZ][currentPosition.y][desiredX] == 1;
	const bool blockedY = actorGrid[checkZ][desiredY][currentPosition.x] == 1;

	/* 한쪽이라도 막혀있으면 이동x */
	return blockedX || blockedY;
}

Node* AStarAlgorithm::FindOpenNode(int x, int y, int z) const
{
	/* 파라미터로 들어온 위치와 같은 위치를 가진 노드가 openList에 이미 있는지 체크 */
	for (Node* node : openList)
	{
		if (node->position == Position(x, y, z)) return node;
	}	
	return nullptr;
}

bool AStarAlgorithm::IsInClosedList(int x, int y, int z) const
{
	/* 파라미터로 들어온 위치와 같은 위치를 가진 노드가 closedList에 이미 있는지 체크 */
	for (Node* node : closedList)
	{ 
		if (node->position == Position(x, y, z)) return true;
	}
	return false;
}

bool AStarAlgorithm::IsDestination(const Node* node) const
{
	return node != nullptr && goalNode != nullptr && node->position == goalNode->position;
}

