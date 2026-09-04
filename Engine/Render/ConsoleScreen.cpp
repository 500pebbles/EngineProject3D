#include "ConsoleScreen.h"

#include <algorithm>
#include <stdexcept>

ConsoleScreen::ConsoleScreen(int width, int height)
    : _width(width), _height(height), _screenBuffer(width * height), _depthBuffer(width * height)
{
    _originalOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    /* 콘솔화면 버퍼 생성 */
    _output = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        CONSOLE_TEXTMODE_BUFFER,
        nullptr);

    /* 버퍼 크기 */
    const COORD bufferSize
    {
        static_cast<SHORT>(_width),
        static_cast<SHORT>(_height)
    };

    SetConsoleScreenBufferSize(_output,bufferSize);

    SMALL_RECT windowRect
    {
        0,
        0,
        static_cast<SHORT>(_width - 1),
        static_cast<SHORT>(_height - 1)
    };

    SetConsoleWindowInfo(_output,TRUE,&windowRect);
    SetConsoleActiveScreenBuffer(_output);

    CONSOLE_CURSOR_INFO cursorInfo{};
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(
        _output,
        &cursorInfo);

    Clear();
}

ConsoleScreen::~ConsoleScreen()
{
    if (_originalOutput != INVALID_HANDLE_VALUE)
    {
        SetConsoleActiveScreenBuffer(_originalOutput);
    }

    if (_output != INVALID_HANDLE_VALUE)
    {
        CloseHandle(_output);
    }
}

int ConsoleScreen::GetWidth() const
{
    return _width;
}

int ConsoleScreen::GetHeight() const
{
    return _height;
}

void ConsoleScreen::Clear()
{
    for (CHAR_INFO& cell : _screenBuffer)
    {
        cell.Char.AsciiChar = ' ';

        cell.Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    /*
     * 원근 투영에서는 1 / z가 클수록
     * 카메라와 가깝다.
     */
    std::fill(_depthBuffer.begin(), _depthBuffer.end(), 0.0f);
}

void ConsoleScreen::SetPixel(int x,int y,float inverseDepth,char character)
{
    if (x < 0 || x >= _width || y < 0 || y >= _height) return;

    const int index = y * _width + x;

    /*
     * 현재 저장된 픽셀보다 카메라에 가까울 때만
     * 새로운 픽셀로 덮어쓴다.
     */
    if (inverseDepth <= _depthBuffer[index]) return;

    _depthBuffer[index] = inverseDepth;
    _screenBuffer[index].Char.AsciiChar = character;
}

void ConsoleScreen::Present()
{
    const COORD bufferSize
    {
        static_cast<SHORT>(_width),
        static_cast<SHORT>(_height)
    };

    const COORD bufferPosition
    {
        0,
        0
    };

    SMALL_RECT writeRegion
    {
        0,
        0,
        static_cast<SHORT>(_width - 1),
        static_cast<SHORT>(_height - 1)
    };

    WriteConsoleOutputA(
        _output,
        _screenBuffer.data(),
        bufferSize,
        bufferPosition,
        &writeRegion);
}