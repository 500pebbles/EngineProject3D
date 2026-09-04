#pragma once
#include "Component/BoxComponent.h"

class ENGINE_API RenderPosition
{
public:
    RenderPosition(int x = 0, int y = 0) noexcept;
    ~RenderPosition() = default;

    /* 렌더러 최종단계에서 RenderVector -> windows.h의 콘솔전용 좌표계로 변환하는 연산자 오버로딩 */
    operator COORD() const;
    operator COORD();
	
public:
    int x = 0;
    int y = 0;
};
