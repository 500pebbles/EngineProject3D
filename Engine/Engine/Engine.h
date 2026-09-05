#pragma once

#include <Core/Core.h>
#include <memory>	

class Renderer3D;
class UWorld;
class ULevel;
class Input;
class Renderer;
class CollisionSystem;

class ENGINE_API Engine  // TODO : 이름 En
{
	struct Setting
	{
		float framerate = 120.f;
		int width = 150;
		int height = 45;
	};

public:
	// 생성자
	Engine();
	virtual ~Engine();

	// 엔진 생명 주기
	void Run();

	void Quit();

	// 월드 불러오기
	template<typename T,
		typename = std::enable_if_t<std::is_base_of<UWorld, T>::value>>
	void AddNewWorld()
	{
		nextWorld = std::make_shared<T>();
	}

	// 엔진 싱글톤 객체 Getter
	static Engine& Get();

	// 엔진 크기
	inline int GetWidth() const { return setting.width; }    // Engine->Renderer->ScreenBuffer까지 넘어가
	inline int GetHeight() const { return setting.height; }  // 그곳에서 최종 콘솔사이즈 결정

protected:
	// 입력 처리 함수 (입력 폴링)
	void ProcessInput();

	// 초기화 함수
	void OnInitialized();
		
	// 엔진 Config 로드
	void LoadEngineSetting();

	/* 게임플레이 이벤트 함수 */
	void BeginPlay();				 // 게임플레이 초기화
	void Tick(float deltaTime);		 // 게임플레이 업데이트
	void Draw();					 // 레벨 그리기
	void ProcessCollision();		 // 충돌 처리
	void SavePreviousInputStates();  //  프레임 간 입력값 저장용
	void Shutdown();				 // 엔진 종료시 리소스 정리		

protected:
	// 엔진 종료요청 플래그
	bool isQuit = false;

	// 엔진 설정 Config 구조체
	Setting setting;
	
	/* 엔진 소유 객체 포인터 */			 
	static Engine* instance;				// 엔진 싱글톤 객체 포인터
	
	std::shared_ptr<UWorld> mainWorld;
	
	std::shared_ptr<UWorld> nextWorld;
	
	std::unique_ptr<Input> input;

	std::unique_ptr<Renderer3D> renderer;	
		
	std::unique_ptr<CollisionSystem> collisionSystem;
	
	
private:
	void CountFPS(float deltaTime); 
	int frameCount = 0; 
	float fpsTimer = 0.0f; 
	int currentFps = 0;
	
};