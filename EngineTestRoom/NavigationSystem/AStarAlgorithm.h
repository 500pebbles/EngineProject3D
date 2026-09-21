#pragma once

#include "Node.h"
#include <vector>

class AStarAlgorithm
{
private:
    struct Direction
    {
        int x;
        int y;
        int z;
        float cost;
    };

    
public:
    AStarAlgorithm();
    ~AStarAlgorithm();
    
private:    
    void Clear();

    /* Path Finder */    
public:
    std::vector<Position> FindPath(
    const Position& startPosition,
    const Position& goalPosition,
    const std::vector<std::vector<std::vector<int>>>& waypointGrid,
    const std::vector<std::vector<std::vector<int>>>& actorGrid);  
    
private:

    Node* CreateNode(const Position& position, Node* parentNode = nullptr);

    std::vector<Position> ConstructPath(Node* destinationNode);

    float CalculateHeuristic(const Position& currentPosition, const Position& goalPosition) const;
    // 외부(겉벽, 하부, 각부) + 다섯 조약돌(재귀변압시설, 메모리 배열, 불운한 발생지, 다섯 조약돌)
    
    Node* FindOpenNode(int x, int y, int z) const;
    
    bool IsValidGrid(const std::vector<std::vector<std::vector<int>>>& grid) const;
    bool IsInRange(int x, int y, int z, const std::vector<std::vector<std::vector<int>>>& grid) const;		
    bool IsDiagonalBlocked(const Position& currentPosition, const Direction& direction,const std::vector<std::vector<std::vector<int>>>& grid) const;
    bool IsInClosedList(int x, int y, int z) const;
    bool IsDestination(const Node* node) const;
    
    /* Visualize */    
public:
    std::vector<Position> GetOpenWaypointPosition() { return openWaypointPositions; }

private:
    std::vector<Node*> allocatedNodes;

    std::vector<Node*> openList;
    std::vector<Node*> closedList;

    Node* startNode;
    Node* goalNode;
    
    std::vector<Position> openWaypointPositions;    
};