#pragma once
#include <iostream>
#include <string>
#include <Windows.h>

class StatTimer
{
public:
    StatTimer()
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

    float GetElapsed()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return static_cast<float>(end - start) / static_cast<float>(frequency.QuadPart);
    }
    
    bool IsElapsed(float seconds)
    {
        CheckEndStat();
        return GetElapsed() >= seconds;
    }

private:
    int64_t start = 0;
    int64_t end = 0;
};
