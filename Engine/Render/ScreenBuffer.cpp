#include "ScreenBuffer.h"
#include <cassert>
#include <iostream>

ScreenBuffer::ScreenBuffer(const RenderPosition& screenSize)
		: size(screenSize)
{
	// 콘솔 버퍼 생성
	buffer = CreateConsoleScreenBuffer(    // CreateConsoleScreenBuffer는 콘솔 데이터 버퍼를 직접 주지않고, 다루도록 핸들을 넘김
	GENERIC_READ | GENERIC_WRITE,          // 읽기/쓰기 권한
	FILE_SHARE_READ | FILE_SHARE_WRITE,    // 다른 프로세스와 공유 
	nullptr,                               // 보안 속성 기본값
	CONSOLE_TEXTMODE_BUFFER,               // 텍스트 기반 버퍼 생성
	nullptr                                // 예약값
	);

	assert(buffer != INVALID_HANDLE_VALUE);

	// 콘솔 크기 설정 : 콘솔은 문자열+\n 개행이 아니라 실제로 가로*세로 사각형 구조를 사용해 RECT가 필요
	SMALL_RECT rect = {};
	rect.Top = 0;
	rect.Left = 0;
	rect.Right = static_cast<short>(size.x - 1);
	rect.Bottom = static_cast<short>(size.y - 1);

	// 핸들과 크기값을 바탕으로 콘솔정보를 설정하는 함수(실제 창 크기)
	BOOL result = SetConsoleWindowInfo(buffer,TRUE, &rect);  
	assert(result == TRUE);

	// 화면 버퍼 크기 설정(창 내부에서 글자들의 한계영역 크기)
	result = SetConsoleScreenBufferSize(buffer, size);         
	assert(result == TRUE);

	// 콘솔 커서정보 받아오고 커서 숨김 설정 
	CONSOLE_CURSOR_INFO info;
	result = GetConsoleCursorInfo(buffer, &info);
	assert(result == TRUE);        
	info.bVisible = FALSE;
	result = SetConsoleCursorInfo(buffer, &info);
	assert(result == TRUE);         
}

ScreenBuffer::~ScreenBuffer()
{
    if (buffer) CloseHandle(buffer);   // Window에서 넘겨받은 핸들 반환
}

void ScreenBuffer::Clear() const
{
	// 콘솔 전체를 리셋. 방식 : 화면의 전체를 공백문자로 덮음
	DWORD writtenCount = 0;
	BOOL result = FillConsoleOutputCharacterA(   // 콘솔의 특정 위치부터 특정 크기만큼 같은 글자로 채우는 함수
		buffer,                                  // Window 콘솔 핸들
		' ',                                     // 채울 문자열 공백
		size.x * size.y,                         // 글자를 채울 크기(길이) 
		RenderPosition(0,0),                        // 시작위치
		&writtenCount                            // 함수는 채운 글자수의 크기를 리턴받아야함
	);
    
	assert(result == TRUE);
}

void ScreenBuffer::Draw(const CHAR_INFO* const charInfo) const
{
	// 설정할 글자 영역, 콘솔은 단순히 긴 문자열+개행이 아니라 실제 가로x세로 사각형의 형태로 존재함
	SMALL_RECT rect = {
		0,
		0,
		static_cast<short>(size.x -1),   // 시작위치가 0부터 시작이므로 -1이 정확한 가로길이 
		static_cast<short>(size.y -1)    // 시작위치가 0부터 시작이므로 -1이 정확한 세로길이
	};
    
	// charInfo는 글자값과 색상이 저장되어있는 1차원 배열이나 2차원 배열처럼 사용 
	BOOL result = WriteConsoleOutputA(      // CHAR_INFO[] 배열에 담긴 문자와 색상정보를 콘솔에 기록하는 WinAPI 
		buffer,                             // Window 콘솔 핸들. CHAR_INFO[]의 데이터를 그대로 출력해줌
		charInfo,                           // CHAR_INFO[] 는 1차원배열이지만, 아래 size의 해석값의 도움으로 2차원으로 그림
		size,						    // 원본 배열(1차원)을 2차원으로 볼때 가로세로 크기
		RenderPosition(0,0),         // 원본 배열에서 복사 시작점
		&rect                               // 콘솔 버퍼에 기록할 사각형
	);
    
	assert(result == TRUE);
}