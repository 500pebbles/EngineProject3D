#pragma once

// LNK4251 경고 끄기 
#pragma warning(disable: 4251)

// 빌드 모드(Export / Import) 에 따라서 변경
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

// 특정 값에 따라 분기하도록 처리
#if ENGINE_BUILD_DLL
#define ENGINE_API DLLEXPORT
#else
#define ENGINE_API DLLIMPORT
#endif