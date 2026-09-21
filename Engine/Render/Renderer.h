#pragma once

#include <Core/Core.h>
#include <Math/Color.h>
#include <string>
#include <vector>
#include <memory>

#include "RenderPosition.h"

class ScreenBuffer;

class ENGINE_API Renderer
{
	/* Frame은 실제 그리기와 무관하고, 모든 글자들의 문자, 색상, 정렬구조를 담고있는 정보집합 */
	struct Frame
	{
		Frame(int bufferCount);                       
		~Frame();
            
		void Clear(const RenderPosition& screenSize);  // 실제 콘솔 클리어가 아닌 문자정보값들을 초기화하는 것
            
		std::unique_ptr<CHAR_INFO[]> charInfoArray;    // 콘솔에 그릴 문자값, CHAR_INFO[]는 문자와 색상을 담는 WinAPI 자료형
		std::unique_ptr<int[]> sortingOrderArray;      // 콘솔에 그릴 문자들의 정렬 순서. 1차원 배열이지만 ScreenBuffer에서 2차원화          
	};

	/* RenderCommand 역시 정보값. 실제로 그리는 주체는 모두 ScreenBuffer가 담당함 */
	struct RenderCommand                  // 화면에 그릴 데이터를 명령 단위로 저장하기 위한 구조체 
	{
		std::string image;									 // 화면에 그릴 문자값            
		RenderPosition position = RenderPosition(0,0);   // 위치            
		Color color = Color::White;							 // 색상            
		int sortingOrder = -1;								 // 정렬순서 (같은 위치에 문자가 겹칠 경우 클수록 우선순위) 
	};
	
	
public:
	Renderer(const RenderPosition& screenSize);
	~Renderer();

	// 화면에 그릴 데이터를 제출하는 함수 Actor의 Draw()에서 제출 -> DrawRenderQueue()에서 드로잉
	void Submit(
		const std::string& image,
		const RenderPosition& position,
		Color color = Color::White,
		int sortingOrder = 0
	); 
	
	void Submit3D(
		);

	// Engine에서 호출할 실질적인 드로우
	void Draw();

	// 싱글톤 접근 Getter
	static Renderer& Get();

private:
	// 그리기 작업을 시작할 때 화면을 클리어하는 함수
	void Clear();

	// 전달받은 렌더 명령을 활용해 화면을 그리는 함수
	void DrawRenderQueue();

	// 그린 결과를 화면에 표시하는 함수
	void Present();

	// 화면버퍼 접근 Getter : *** 실질적으로 콘솔에 글자를 출력하는 주체 ***
	const ScreenBuffer* const GetCurrentBuffer() const;

private:
	// Renderer 싱글톤 포인터
	static Renderer* instance;

	// 이번 프레임에 렌더링할 '명령'들을 모아두는 배열
	std::vector<RenderCommand> renderQueue;

	// 화면 크기
	RenderPosition screenSize;

	// 드로우 글자/정렬순서 2차원 배열을 관리하는 Frame 객체
	std::unique_ptr<Frame> frame;

	// 이중버퍼 구현을 위한 화면버퍼 2개
	std::unique_ptr<ScreenBuffer> screenBufferArray[2];

	// 화면 버퍼 인덱스
	int currentBufferIndex = 0;
};