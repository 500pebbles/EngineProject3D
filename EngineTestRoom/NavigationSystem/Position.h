#pragma once

class  Position
{
public:
    Position(int x = 0, int y = 0, int z = 0)
        : x(x), y(y), z(z)
    {
        
    }
    
    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }
    
public:
    int x;
    int y;
    int z;
};
