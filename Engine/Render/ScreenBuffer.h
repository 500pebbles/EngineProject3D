#pragma once

#include <Windows.h>

#include "RenderPosition.h"

class ScreenBuffer
{
public:
	ScreenBuffer(const RenderPosition& screenSize);
	~ScreenBuffer();

	void Clear() const;
	void Draw(const CHAR_INFO* const charInfo) const;

	inline HANDLE GetBuffer() const { return buffer; }

private:
	HANDLE buffer = nullptr;

	RenderPosition size;
};