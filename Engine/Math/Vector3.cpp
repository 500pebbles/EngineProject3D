#include "Vector3.h"

#include <cmath>

Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);


Vector3::Vector3(float x, float y, float z)
    : x(x), y(y), z(z)
{
}

float Vector3::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalized() const
{
    const float length = Length();
    if (length == 0.0f) return {};
    return {x / length, y / length, z / length};
}

float Vector3::Dot(const Vector3& other) const
{
    return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(const Vector3& other) const
{
    return {y * other.z - z * other.y,z * other.x - x * other.z,x * other.y - y * other.x};
}

Vector3 Vector3::RotateX(float radians) const
{
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    return { x, y * cosine - z * sine, y * sine + z * cosine };
}

Vector3 Vector3::RotateY(float radians) const
{
    const float cosine = std::cos(radians);
    const float sine = std::sin(radians);

    return { x * cosine - z * sine, y, x * sine + z * cosine };
}

Vector3 Vector3::operator+(const Vector3& other) const
{
    return { x + other.x, y + other.y, z + other.z };
}

Vector3 Vector3::operator-(const Vector3& other) const
{
    return { x - other.x,y - other.y,z - other.z };
}

Vector3 Vector3::operator*(float scalar) const
{
    return { x * scalar,y * scalar,z * scalar };
}

bool Vector3::operator==(const Vector3& other) const
{
    return std::fabs(x - other.x) <= KINDA_SMALL_NUMBER &&
           std::fabs(y - other.y) <= KINDA_SMALL_NUMBER &&
           std::fabs(z - other.z) <= KINDA_SMALL_NUMBER;
}
