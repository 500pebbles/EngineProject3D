#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <Core/Core.h>  
#include <vector>

class ENGINE_API ConsoleScreen
{
public:
    ConsoleScreen(int width, int height);
    ~ConsoleScreen();

    ConsoleScreen(const ConsoleScreen&) = delete;
    ConsoleScreen& operator=(const ConsoleScreen&) = delete;

    int GetWidth() const;
    int GetHeight() const;

    void Clear();

    void SetPixel(
        int x,
        int y,
        float inverseDepth,
        char character);

    void Present();

private:
    int _width;
    int _height;

    HANDLE _originalOutput = INVALID_HANDLE_VALUE;
    HANDLE _output = INVALID_HANDLE_VALUE;

    std::vector<CHAR_INFO> _screenBuffer;
    std::vector<float> _depthBuffer;
};