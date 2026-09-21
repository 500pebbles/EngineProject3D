#pragma once
#include "Position.h"

class  Node
{
public:
    Node(const Position& position, Node* parentNode = nullptr)
    : position(position), parentNode(parentNode)
    {
        
    }
    
    
public:
    Position position;
    float gCost = 0.f;
    float hCost = 0.f;
    float fCost = 0.f;
    
    Node* parentNode;
};