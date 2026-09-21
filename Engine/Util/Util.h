#pragma once

#include <random>

// 랜덤 엔진 : 프로그램 중 하나 존재하는 여러 종류의 랜덤생성기
inline std::mt19937& GetRandomEngine()
{
    static std::mt19937 engine;
    return engine; 
}

// 랜덤 엔진에 필요한 시드값. 랜덤 엔진은 무작위처럼 보이지만 사실 시드기반
inline void SetRandomSeed()
{		
    std::random_device randomDevice;			// 시드값에 쓸 하드웨어 기반 난수 생성
    GetRandomEngine().seed(randomDevice()); // 엔진에 시드 설정
}

// min ~ max 사이 무작위 정수 반환
inline int RandomRange(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);

    return distribution(GetRandomEngine());
}

// min ~ max 사이 무작위 실수 반환
inline float RandomRange(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);

    return distribution(GetRandomEngine());
}