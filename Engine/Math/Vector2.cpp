#include "Vector2.h"
#include <cassert>
#include <cmath>

Vector2 Vector2::Zero(0.f, 0.f);
Vector2 Vector2::One(1.f, 1.f);
Vector2 Vector2::Right(1.f, 0.f);
Vector2 Vector2::Up(0.f, -1.f);

Vector2::Vector2(float x, float y) noexcept
	: x(x), y(y)
{
}

Vector2 Vector2::operator+(const Vector2& other) const
{
	return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const
{
	return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(const Vector2& other) const
{
	return Vector2(x * other.x, y * other.y);
}

Vector2 Vector2::operator/(const Vector2& other) const
{
	assert(std::fabs(other.x) > KINDA_SMALL_NUMBER);
	assert(std::fabs(other.y) > KINDA_SMALL_NUMBER);
	
	return Vector2(x / other.x, y / other.y);
}

Vector2& Vector2::operator=(const Vector2& other)
{
	x = other.x;
	y = other.y;

	return *this;
}

bool Vector2::operator==(const Vector2& other) const
{
	return std::fabs(x - other.x) <= KINDA_SMALL_NUMBER &&
		   std::fabs(y - other.y) <= KINDA_SMALL_NUMBER;
}

bool Vector2::operator!=(const Vector2& other) const
{
	return !(*this == other); 
}