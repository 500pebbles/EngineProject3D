#pragma once
#include "Vector3.h"
#include <Core/Core.h>

class  ENGINE_API Matrix4
{
public:
    Matrix4();
    Matrix4(const Matrix4& other);
    ~Matrix4() = default;

    // 전치(Transpose).
    static Matrix4 Transpose(const Matrix4& matrix);

    // 역행렬(Inverse).
    static Matrix4 Inverse(const Matrix4& matrix);

    // 아핀 변환(Affine Transformation).
    // 동차좌표계(Homogeneous Coordinates).

    // 이동 행렬.
    static Matrix4 Translation(float x, float y, float z);
    static Matrix4 Translation(const Vector3& position);

    // 회전 행렬.
    static Matrix4 Rotation(float x, float y, float z);
    static Matrix4 Rotation(const Vector3& rotation);
    static Matrix4 RotationX(float angle);
    static Matrix4 RotationY(float angle);
    static Matrix4 RotationZ(float angle);

    // 스케일 변환 행렬.
    static Matrix4 Scale(float x, float y, float z);
    static Matrix4 Scale(const Vector3& scale);
    static Matrix4 Scale(float scale);

    // 카메라 변환 행령(뷰 변환).
    static Matrix4 LookAt(const Vector3& position, const Vector3& target, const Vector3& up);

    // 원근 투영 행렬(Perspective).
    static Matrix4 Perspective(
        float fieldOfView,		// 시야각.
        float width,			// 화면 너비.
        float height,			// 화면 높이.
        float nearDistance,		// 뷰 프러스텀 가까운 위치.
        float farDistance);		// 뷰 프러스텀 먼 위치.

    // 연산자 오버로딩.
    Matrix4& operator=(const Matrix4& other);
    Matrix4 operator*(const Matrix4& other) const;
    Matrix4& operator*=(const Matrix4& other);

    // 벡터와의 연산 오버로딩.
    friend Vector3 operator*(const Matrix4& m, const Vector3& v);
    friend Vector3 operator*(const Vector3& v, const Matrix4& m);

    // 전역 변수.
    // 단위 행렬.
    static Matrix4 Identity;

    // 호도법(라디안) <-> 각도법 변환 공식.
    static constexpr float degreeToRadian = 3.141592f / 180.0f;
    static constexpr float radianToDegree = 180.0f / 3.141592f;

private:
    // 공용체.
    // 메모리 공간 공유. 
    // 여러 변수 중에 가장 공간이 큰 메모리를 할당 후 공유.
    union
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };

        // 배열.
        float elements[4 * 4] = {};
    };
};
