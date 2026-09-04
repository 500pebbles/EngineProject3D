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
    bool operator==(const Vector3& other) const;
    
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
    
    static Vector3 Zero;
    static Vector3 One;
    static Vector3 Right;
    static Vector3 Up;
    static Vector3 Forward;
    
    float KINDA_SMALL_NUMBER = 0.0001f;
};