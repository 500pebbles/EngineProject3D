#pragma once

#include <Core/Core.h>
#include "Actor/Actor.h"
#include "Component/BoxComponent.h"

class Vector2;

class ENGINE_API Input
{
	friend class Engine;

	/* 키 입력 상태를 저장하기 위한 구조체 */
	struct KeyState
	{
		bool isKeyDown = false;   // 현재 프레임에 키가 눌렸는가?          
		bool wasKeyDown = false;  // 이전 프레임에 키가 눌렸는가?
	};

public:
	Input();
	~Input();

	/* 키 입력 & 해제 여부 확인 */
	bool GetKeyDown(int keyCode) const;   // 이전 프레임에 안 눌렀다가, 이번 프레임에 눌렀다면 true
	bool GetKeyUp(int keyCode) const;     // 이전 프레임에 눌렀다가, 이번 프레임에 안 눌렀다면 true
	bool GetKey(int keyCode) const;       // 현재 프레임에 입력이 감지되면 반복해서 true
		
	// 마우스의 콘솔상 좌표를 반환
	const Vector2& GetMousePosition() const { return mousePosition; }

	// Input 객체 싱글톤 접근 
	static Input& Get();

private:
	/* Input값 저장 */
	void ProcessInput(); // 현재 프레임에 특정 키 입력이 발생했는지 처리		
	void SavePreviousInput(); // 이전 프레임의 키 눌림 상태를 저장

private:
	/* Input 객체 싱글톤 */
	static Input* instance;
		
	/* Windows에서 매핑한 가상 키 */
	const int keyCount = 256;        // 가상 키의 수
	KeyState keyStates[256] = { };   // 키 상태를 관리할 배열    
		
	/* 콘솔 Input 관련된 옵션 */
	HANDLE inputHandle = INVALID_HANDLE_VALUE;  // 윈도우 콘솔에 입력하는 이벤트를 읽기 위한 핸들
	DWORD originalConsoleMode = 0;				// 프로그램 시작 시 설정되어 있던 콘솔 입력 모드 (설정값 비트 ON/OFF)
	bool shouldRestoreConsoleMode = false;		// 종료할 때 기존 콘솔 입력 모드를 복구할지 여부
				
	/* 마우스 옵션 */
	Vector2 mousePosition = Vector2::Zero; // 마우스 포인터의 콘솔상 현재 좌표
	
	
/* Additional Mouse Tracking */
public:
	const Vector2& GetMouseDelta() const;
	void SetMouseCaptured(bool captured);

private:
	void UpdateMouseDelta();

private:
	Vector2 mouseDelta = {};
	bool isMouseCaptured = false;
	bool isMouseCentered = false;
	bool hasConsoleFocus = true;
};