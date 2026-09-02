#include "RenderPosition.h"

RenderPosition::RenderPosition(int x, int y) noexcept
    : x(x), y(y)
{
}

RenderPosition::operator _COORD() const
{
    COORD coord = {};
    coord.X = static_cast<short>(x);
    coord.Y = static_cast<short>(y);

    return coord;
}

RenderPosition::operator _COORD()
{
    COORD coord = {};
    coord.X = static_cast<short>(x);
    coord.Y = static_cast<short>(y);

    return coord;
}
