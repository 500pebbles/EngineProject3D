#pragma once

#include <Core/Core.h>
#include <Windows.h>

enum class ENGINE_API Color : WORD
{
		
    Red   = FOREGROUND_RED,
    Green = FOREGROUND_GREEN,
    Blue  = FOREGROUND_BLUE,

    Yellow = Red | Green,
    Cyan   = Green | Blue,
    Purple = Red | Blue,
    White  = Red | Green | Blue,

    BrightRed   = Red   | FOREGROUND_INTENSITY,
    BrightGreen = Green | FOREGROUND_INTENSITY,
    BrightBlue  = Blue  | FOREGROUND_INTENSITY,

    BrightYellow = Yellow | FOREGROUND_INTENSITY,
    BrightCyan   = Cyan   | FOREGROUND_INTENSITY,
    BrightPurple = Purple | FOREGROUND_INTENSITY,
    BrightWhite  = White | FOREGROUND_INTENSITY,
    Gray = FOREGROUND_INTENSITY
};