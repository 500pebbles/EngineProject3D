#pragma once

#include <Core/Core.h>
#include <Windows.h>

class ENGINE_API Vector2
{
public:
	Vector2(float x = 0.f, float y = 0.f) noexcept;
	~Vector2() = default;

	/* 연산자 오버로딩 */
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;
	Vector2 operator/(const Vector2& other) const;

	Vector2& operator=(const Vector2& other);

	bool operator==(const Vector2& other) const;
	bool operator!=(const Vector2& other) const;

	/* 자주쓰는 값 설정 */
	static Vector2 Zero;
	static Vector2 One;
	static Vector2 Right;
	static Vector2 Up;
	
	float KINDA_SMALL_NUMBER = 0.0001f;

public:
	float x = 0.f;
	float y = 0.f;
};