#include "Input.h"
#include <cassert>
#include <Windows.h>

// static 변수 초기화.
Input* Input::instance = nullptr;

Input::Input()
{
	/* Part 1 : 현재 생성중인 Input 객체를 싱글톤 인스턴스로 등록 */
	assert(!instance && "instance should be null here.");
	instance = this;

	
	/* Part 2 : InputHandle 정보값 등록 */
	inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (inputHandle == INVALID_HANDLE_VALUE || inputHandle == nullptr) return;  // 콘솔획득 실패시 리턴	
	
	if (GetConsoleMode(inputHandle, &originalConsoleMode)) // 현재 콘솔 입력 모드를 가져옴
	{
		// 현재 입력 모드를 가져와서, 비트연산(DWORD 자리수별)으로 기능들을 끄거나 켬
		DWORD inputMode = originalConsoleMode;
		inputMode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;  // 마우스 Input 허용
		inputMode &= ~ENABLE_QUICK_EDIT_MODE;					  // 마우스 드래그옵션 비허용

		// 변경을 완료하고 현재 InputHandle에 변경된 옵션 적용
		shouldRestoreConsoleMode = SetConsoleMode(inputHandle, inputMode) != FALSE;
	}
}

Input::~Input()
{
	// 입력 모드 변경에 성공했으면 기존 콘솔 입력 모드로 복구
	if (shouldRestoreConsoleMode) SetConsoleMode(inputHandle, originalConsoleMode);

	// 싱글톤 접근객체 정리
	instance = nullptr;
}

bool Input::GetKeyDown(int keyCode) const
{
	return !keyStates[keyCode].wasKeyDown
		&& keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyUp(int keyCode) const
{
	return keyStates[keyCode].wasKeyDown
		&& !keyStates[keyCode].isKeyDown;
}

bool Input::GetKey(int keyCode) const
{
	return keyStates[keyCode].isKeyDown;
}

Input& Input::Get()
{
	assert(instance && "instance should not be null here");
	return *instance;
}

void Input::ProcessInput()
{
	if (inputHandle == INVALID_HANDLE_VALUE || inputHandle == nullptr) return;

	// 한 번에 읽어올 콘솔 입력 이벤트 배열
	INPUT_RECORD records[128] = { };   // KEY_EVENT + MOUSE_EVENT + FOCUS_EVENT

	// 콘솔 입력 버퍼에 대기 중인 이벤트 수를 넘겨받을거 (비트플래그 x 그냥 정수형)
	DWORD pendingEventCount = 0;  

	// 한 프레임 동안 입력 버퍼에 쌓인 이벤트를 모두 처리.
	while (GetNumberOfConsoleInputEvents(inputHandle, &pendingEventCount) && pendingEventCount > 0)
	{
		// 실제로 읽어온 이벤트 수를 저장할 변수.
		DWORD readEventCount = 0;

		// 배열 크기를 넘지 않도록 한 번에 읽을 이벤트 수 결정.
		const DWORD readCount = pendingEventCount < 128 ? pendingEventCount : 128;

		// 콘솔 입력 버퍼에서 이벤트 읽기.
		if (!ReadConsoleInput(inputHandle, records, readCount, &readEventCount))
		{
			break;
		}

		// 읽어온 입력 이벤트를 순서대로 처리.
		for (DWORD ix = 0; ix < readEventCount; ++ix)
		{
			// 현재 처리할 입력 이벤트.
			const INPUT_RECORD& record = records[ix];

			// 입력 이벤트 종류에 따라 처리.
			switch (record.EventType)
			{
			/* 키보드 이벤트 KeyState 처리 */
			case KEY_EVENT:
			{					
				const KEY_EVENT_RECORD& keyEvent = record.Event.KeyEvent;
				const WORD keyCode = keyEvent.wVirtualKeyCode;
				if (keyCode < keyCount)
				{
					KeyState& state = keyStates[keyCode];
					const bool isKeyDown = keyEvent.bKeyDown != FALSE;
					state.isKeyDown = isKeyDown;
				}
				break;
			}
			
			/* 마우스 이벤트 KeyState 처리 */
			case MOUSE_EVENT:
			{					
				const MOUSE_EVENT_RECORD& mouseEvent = record.Event.MouseEvent;

				mousePosition.x = mouseEvent.dwMousePosition.X;
				mousePosition.y = mouseEvent.dwMousePosition.Y;

				// 마우스 버튼과 가상 키 코드를 연결하기 위한 구조체.
				const struct MouseButton
				{
					// 키 상태 배열에서 사용할 가상 키 코드.
					int keyCode;

					// 마우스 이벤트에서 버튼 상태를 확인할 비트 값.
					DWORD buttonMask;
				} mouseButtons[] = {
					{ VK_LBUTTON, FROM_LEFT_1ST_BUTTON_PRESSED }, // 좌클
					{ VK_RBUTTON, RIGHTMOST_BUTTON_PRESSED },		// 우클
					{ VK_MBUTTON, FROM_LEFT_2ND_BUTTON_PRESSED }	// 휠
				};

				// 왼쪽, 오른쪽, 가운데 마우스 버튼 상태 처리.
				for (const MouseButton& button : mouseButtons)
				{
					// 버튼이 눌렸는지 비트 연산으로 확인한 후 키 상태에 저장.
					KeyState& state = keyStates[button.keyCode];
					const bool isKeyDown =
						(mouseEvent.dwButtonState & button.buttonMask) != 0;
					state.isKeyDown = isKeyDown;
				}
				break;
			}

			/* 콘솔 창이 Focus 되지 않을경우 입력 방지 */
			case FOCUS_EVENT:					
				if (!record.Event.FocusEvent.bSetFocus) // 콘솔 창이 입력 포커스를 잃었는지 확인.
				{
					// 포커스를 잃는 동안 KeyUp 이벤트가 누락되어
					// 키가 계속 눌린 상태로 남는 것을 방지.
					for (KeyState& state : keyStates)
					{
						state.isKeyDown = false;
					}
				}
				break;
			}
		}
	}
}
	
void Input::SavePreviousInput()
{
	for (KeyState& state : keyStates) // 한 프레임이 끝날 때, 해당프레임에 눌렸던 키를 전부 저장함
	{
		// 가상 키가 매핑한 256개의 모든 키에 대해 입력을 검사하고, 구조체의 is/was 플래그 중 was를 설정함
		state.wasKeyDown = state.isKeyDown;
	}
}		