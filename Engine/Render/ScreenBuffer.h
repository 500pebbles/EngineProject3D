#pragma once

#include <Windows.h>

#include "RenderPosition.h"

// 이중 버퍼로 구현, 콘솔 핸들을 관리해 실질적인 출력을 담당함
class ScreenBuffer
{
public:
	ScreenBuffer(const RenderPosition& screenSize);
	~ScreenBuffer();

	// 화면 클리어
	void Clear() const;

	// 전달된 글자 값 그리기
	void Draw(const CHAR_INFO* const charInfo) const;

	// Window 핸들 접근 Getter
	inline HANDLE GetBuffer() const { return buffer; }

private:
	// 화면 버퍼 핸들 : 콘솔은 Window가 담당하기에 직접 접근할 수 없어 핸들을 통한 접근
	HANDLE buffer = nullptr;

	// 화면 크기
	RenderPosition size;
};