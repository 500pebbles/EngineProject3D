#pragma once

#include "Camera/Camera.h"
#include "ConsoleScreen.h"
#include "Math/Matrix4.h"
#include "Shape/Mesh.h"
#include <Core/Core.h>  

#include "ScreenBuffer.h"


class ENGINE_API Renderer3D
{
    struct RenderCommand
    {
        const Mesh* mesh = nullptr;
        Matrix4 worldMatrix;
    };
    
private:
    struct ScreenVertex
    {
        float x = 0.0f;
        float y = 0.0f;

        float inverseDepth = 0.0f;
        
    };
    
public:
    Renderer3D(ConsoleScreen& screen);

    void Render(const Mesh& mesh, float rotationAngle);

    ScreenVertex Project(const Vector3& viewPosition) const;

    void DrawTriangle(const ScreenVertex& v0,const ScreenVertex& v1,const ScreenVertex& v2,char character);

    static float Edge(const ScreenVertex& start,const ScreenVertex& end,float x,float y);

    static char GetShadeCharacter(float brightness);
    

private:
    ConsoleScreen& _screen;
    
    Camera _camera;
    float _focalLength = 1.0f; 
    
private:
    std::vector<RenderCommand> _renderCommands;
    
private:
    // 이중버퍼 구현을 위한 화면버퍼 2개
    std::unique_ptr<ScreenBuffer> screenBufferArray[2];

    // 화면 버퍼 인덱스
    int currentBufferIndex = 0;
};
