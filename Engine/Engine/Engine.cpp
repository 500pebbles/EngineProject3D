#include "Engine.h"
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Physics/CollisionSystem.h>
#include <iostream>
#include <Windows.h>
#include <cassert>
#include "World/World.h"

Engine* Engine::instance = nullptr;

Engine::Engine()
{
	assert(!instance && "instance is not null");
	instance = this;

	LoadEngineSetting();

	input = std::make_unique<Input>();
	renderer = std::make_unique<Renderer>(RenderPosition(setting.width, setting.height));
	collisionSystem = std::make_unique<CollisionSystem>();
}

Engine::~Engine()
{
	instance = nullptr;
}

void Engine::Run()
{
	/* 프레임 제한을 위해 엔진의 반복문 내부에서 쓸 고해상도 타이머 셋업 */
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	int64_t current = counter.QuadPart;
	int64_t previous = current;
	float oneFrameTime = 1.0f / setting.framerate;	 // 프레인 제한값

	
	while (true)
	{
		if (isQuit) break;

		/* 프레임 제한을 위한 처리 */
		QueryPerformanceCounter(&counter);
		current = counter.QuadPart;
		float deltaTime = static_cast<float>(current - previous) / static_cast<float>(frequency.QuadPart);

		/* 메인 프로세스 */
		if (deltaTime >= oneFrameTime)
		{
			ProcessInput();
			
			OnInitialized();

			BeginPlay();

			Tick(deltaTime);

			ProcessCollision();

			Draw();
			

			/* Draw가 끝나는 순간 현재 프레임 처리는 완료됨. 
			 * 이하 작업은 다음 프레임을 위한 작업들 
			 * 레벨 교체 요청, 액터 추가삭제 요청, 현재프레임 Input저장 */
			
			
			if (nextWorld)    // 월드 교체
			{
				if (mainWorld) mainWorld.reset();

				mainWorld = nextWorld;
				nextWorld.reset();
			}
			
			if (mainWorld)	  // 액터 추가삭제
			{
				mainWorld->ProcessAddAndDestroyActors();
			}

			SavePreviousInputStates();  // 현재 프레임의 키 입력 정보들을 전부 저장 (다음 프레임과 비교를 위해)

			previous = current;		    // deltaTime 설정을 위해 마지막 업데이트카운트 기록
		}
	}

	Shutdown();	 // isQuit 플래그로 탈출시 엔진 종료
}

void Engine::Quit()
{
	isQuit = true;
}

Engine& Engine::Get()
{
	assert(instance && "instance is null");
	return *instance;
}

void Engine::ProcessInput()
{
	assert(input && "input should not be null here");
	if (!input) return;
	input->ProcessInput();
}

void Engine::OnInitialized()
{
	if (!mainWorld || mainWorld->HasInitialized()) return;
	mainWorld->OnInitialized();
}

void Engine::BeginPlay()
{
	if (!mainWorld) return;
	mainWorld->BeginPlay();
}

void Engine::Tick(float deltaTime)
{
	if (!mainWorld) return;
	mainWorld->Tick(deltaTime);
}

void Engine::Draw()
{
	/* 
	 * Draw 순서 
	 * 레벨의 Draw가 호출되면, 보유한 액터를 모두 순회하며 Draw 호출
	 * 액터의 Draw가 호출되면, 본인이 보유한 드로우 데이터들을 수집해 renderQueue렌더러에게 Submit
	 * 렌더러의 Draw가 호출되면, renderQueue의 정보를 바탕으로 화면에 드로우 (상세는 Renderer.cpp Draw() 참조) 
	 */
	
	// 월드가 보유한 레벨의 Draw 이벤트 호출 : 액터들이 "정보값"을 renderQueue에 주입함
	if (!mainWorld) return;
	mainWorld->Draw();		 
	
	// 렌더러의 Draw 이벤트 호출 : renderQueue의 정보값을 바탕으로 실질적으로 화면에 "출력"
	if (!renderer) return;
	renderer->Draw();
}

void Engine::ProcessCollision()
{
	if (!mainWorld || !collisionSystem) return;
	collisionSystem->ProcessCollision(mainWorld->persistentLevel->actors); // 의존성 주입 (Dependency Injection)
}

void Engine::SavePreviousInputStates()
{
	assert(input && "input should not null here");
	if (!input) return;
	input->SavePreviousInput();
}

void Engine::Shutdown()
{
}

void Engine::LoadEngineSetting()
{
	// Setting.txt 파일 열기
	FILE* file = nullptr;
	fopen_s(&file, "../Config/Setting.txt", "rt");
	if (!file)
	{
		std::cout << "Failed to open engine setting file.\n";
		__debugbreak();
		return;
	}

	// 버퍼 준비
	const int bufferSize = 2048;
	char buffer[bufferSize] = {};

	// 버퍼에 문자열 읽어오기
	size_t readSize = fread(buffer, sizeof(char), bufferSize, file);

	// 문자열을 자르며 해석
	char* context = nullptr;
	char* token = nullptr;
	token = strtok_s(buffer, "\n", &context);  // '\n' 기준으로 잘라 왼쪽은 token, 남은 문자열 전체는 context

	while (token)
	{
		// token을 읽어가며 공백이 나오기 전까지의 문자를 key에 저장(공백은 제외)
		char key[15] = {};	
		sscanf_s(token, "%s", key, 15);
		
		// key에 저장된 문자열과 구조체의 변수명을 비교후 대입
		if (strcmp(key, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &setting.framerate);
		}
		else if (strcmp(key, "width") == 0)
		{
			sscanf_s(token, "width = %d", &setting.width);
		}
		else if (strcmp(key, "height") == 0)
		{
			sscanf_s(token, "height = %d", &setting.height);
		}
		
		// 나머지 문자열을 다시 '\n' 기준으로 자르고 token이 완전히 비워질때까지 반복
		token = strtok_s(nullptr, "\n", &context);			
	}
	
	fclose(file);
	file = nullptr;		
}