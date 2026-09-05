#include "Renderer.h"
#include "ScreenBuffer.h"
#include <cassert>
#include <iostream>
#include <Windows.h>

// -------------------------- Frame -------------------------- //
Renderer::Frame::Frame(int bufferCount)
{
    // 프레임을 이루는 두 요소 객체 생성후 포인터 획득
	charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
	sortingOrderArray = std::make_unique<int[]>(bufferCount);
}

Renderer::Frame::~Frame()
{
}

void Renderer::Frame::Clear(const RenderPosition& screenSize)
{
	const int width = screenSize.x;
	const int height = screenSize.y;

	// 프레임을 이루는 두 요소 배열을 순회하며 클리어
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
            // 개념적으로는 2차원 배열이지만 PC 구조는 1차원 배열을 원하기 때문에 배열변환
			const int index = (y * width) + x;

			// 글자와 정렬순서 초기화
			CHAR_INFO& info = charInfoArray[index];
			info.Char.AsciiChar = ' ';
			info.Attributes = 0;            // 색상 표기안함(투명)
			sortingOrderArray[index] = -1;  // 정렬값 초기화
		}
	}
}


// -------------------------- Renderer -------------------------- //    	

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(const RenderPosition& screenSize)
	: screenSize(screenSize)
{
    // 객체 생성시 생성자에서 기존객체 검사 후 포인터 반환
	assert(!instance && "instance should be null");
	instance = this;
	
    // Frame 객체 생성
	const int bufferCount = screenSize.x * screenSize.y;  
	frame = std::make_unique<Frame>(bufferCount);  // Frame은 화면에 채울 문자 전체를 보유하기에 화면크기 = 배열크기
	frame->Clear(screenSize);                      // 생성 후 혹시 있을 프레임 지우기

	
	// 이중 버퍼 구현을 위한 콘솔 버퍼 생성 및 초기화
	screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
	screenBufferArray[0]->Clear();
	screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
	screenBufferArray[1]->Clear();

	// 0번 버퍼 화면에 띄우기
	SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetBuffer()); // 화면을 띄우는건 렌더러지만, 데이터는 ScreenBuffer
}

Renderer::~Renderer()
{
	instance = nullptr;

	// 콘솔 창 원래대로 복구
	SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
}

void Renderer::Submit(const std::string& image, const RenderPosition& position, Color color, int sortingOrder)
{
	// 랜더 명령 구조체 생성
	RenderCommand command;
	command.image = image;
	command.position = position;
	command.color = color;
	command.sortingOrder = sortingOrder;
    
	// 랜더 큐에 명령 추가
	renderQueue.emplace_back(command);
}

void Renderer::Draw()
{
	/* 드로잉 파이프라인 구조는 
	 * 액터가 Submit()으로 보낸 renderCommand 정보값을 renderQueue 리스트에 저장 
	 * renderQueue가 갖고있는 정보값을, WinAPI가 읽을수있는 자료형(CHAR_INFO[], INT[])을 가진 Frame에 변환해 저장 
	 * Frame이 가진 변환된 배열을 기반으로 현재 인덱스의 ScreenBuffer의 핸들이 실질적으로 콘솔에 드로우 */
             
	// 기존 화면 클리어 : Frame(정보값) 클리어 -> ScreenBuffer가 모든 문자열 공백화
	Clear();
    
	// 프레임 그리기 : 위 파이프라인 참조
	DrawRenderQueue();
    
	// 화면에 표시 : 두개의 인덱스중 어느 ScreenBuffer를 화면에 표시할 지 결정
	Present();
}

Renderer& Renderer::Get()
{
	assert(instance && "instance should not be null");
	return *instance;
}

void Renderer::Clear()
{
	// 프레임 초기화 호출
	frame->Clear(screenSize);

	// 콘솔 버퍼 초기화
	GetCurrentBuffer()->Clear();
}

void Renderer::DrawRenderQueue()
{
    // RenderQueue를 순회하며 그리기 명령 실행
	for (const RenderCommand& command : renderQueue)
	{
		/* Step 1 : 세부위치조정 */
		
		// 인덱스가 그릴 문자값이 없으면 스킵
		if (command.image.empty()) continue;
		
		// y 위치가 화면을 벗어났다면 스킵
		if (command.position.y <0 || command.position.y >= screenSize.y) continue;

		// x 위치가 화면을 벗어났다면 건너뛰기
		const int length = static_cast<int>(command.image.length());   // 그리려는 문자열 길이 값                       
		const int startX = command.position.x;                         // 글자의 시작 위치 (가장 왼쪽)      
		const int endX = startX + length - 1;                          // 글자의 끝 위치 (가장 오른쪽)                     
		if (endX < 0 || startX >= screenSize.x) continue;

		// 화면에 일부 가려질 경우 실제 그릴 글자의 위치 구하기
		const int visibleStartX = startX < 0 ? 0 : startX;
		const int visibleEndX = endX >= screenSize.x ? screenSize.x -1 : endX;
		
		
		/* Step 2 : 정리된 command의 위치값을 1차원 배열로 변환함 */

        // 문자열을 루프 순회하며 글자를 콘솔(2차원 배열)에 하나씩 정보값 주입
		for (int x = visibleStartX; x <= visibleEndX; ++x)
		{
            // 문자열에서 글자값을 가져올 때 사용할 인덱스
			const int sourceIndex = x - startX;

            // 글자 2차원 배열의 인덱스
			const int index = (command.position.y * screenSize.x) + x;             // 2차원 -> 1차원 배열로 변환

			// 그리기 전 정렬순서를 비교해 드로우 여부를 판정
			if (frame->sortingOrderArray[index] > command.sortingOrder) continue;  // 기본값 -1, 이미 그려진 개체는 정렬순서가 있으므로(초기화 0) 비교

			// 위치 설정 끝난 후 글자값, 색상 설정
			frame->charInfoArray[index].Char.AsciiChar = command.image[sourceIndex];     
			frame->charInfoArray[index].Attributes = static_cast<DWORD>(command.color);

			// 글자 정렬기준 설정
			frame->sortingOrderArray[index] = command.sortingOrder;                
		}
	}
		 
	/* Step 3 : 변환된 배열을 스크린버퍼에 넘겨주며 실질적인 콘솔에 드로잉 */

    // 글자 2차원배열 정보값을 바탕으로, 현재 ScreenBuffer를 불러와 콘솔에 드로우
	GetCurrentBuffer()->Draw(frame->charInfoArray.get());

	// renderQueue 비우기
	renderQueue.clear();
        
	// 콘솔 색상 초기화
	SetConsoleTextAttribute(GetCurrentBuffer()->GetBuffer(), static_cast<DWORD>(Color::White));         
}

void Renderer::Present()
{
	/* 콘솔은 여러 개의 화면 버퍼를 만들 수 있지만, 하나만 활성 상태가 될수있고, 그게 화면에 보이는 출력화면
       GetCurrentBuffer()는 Renderer가 보유한 두개의 스크린버퍼중 하나. 이 핸들을 가져오고, 직후 버퍼 인덱스를 스왑 */		
	
	SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());	// 화면에 출력될 데이터 버퍼 설정
	currentBufferIndex = 1 - currentBufferIndex;				    // 출력 직후 버퍼 인덱스 교체        
}

const ScreenBuffer* const Renderer::GetCurrentBuffer() const
{
	return screenBufferArray[currentBufferIndex].get();
}		