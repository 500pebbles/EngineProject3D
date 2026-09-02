#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <Core/Core.h>

class ENGINE_API StatTimer
{
public:
    StatTimer(const std::string& tag)
        : tag(tag)
    {
    }
    
public:
    void CheckStartStat()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        start = counter.QuadPart;
    }

    void CheckEndStat()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        end = counter.QuadPart;
    }

    void Reset()
    {
        start = end = 0;
    }

    void PrintResult()
    {
        std::cout << "[" << tag << "] elapsed: " << GetElapsed() << "\n";
    }

private:
    float GetElapsed()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return static_cast<float>(end - start) / static_cast<float>(frequency.QuadPart);
    }

private:
    std::string tag;
    int64_t start = 0;
    int64_t end = 0;
};
