#pragma once
#include <Core/Core.h>

class ENGINE_API Vector3
{
public:
    Vector3(float x = 0.f, float y = 0.f, float z = 0.f);

public:
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    
public:
    float Length() const;    
    Vector3 Normalized() const;   
    
    float Dot(const Vector3& other) const;
    Vector3 Cross(const Vector3& other) const; 
    
    /* TODO: SRT 변환행렬로 전환 */
    Vector3 RotateX(float radians) const;
    Vector3 RotateY(float radians) const;

public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};