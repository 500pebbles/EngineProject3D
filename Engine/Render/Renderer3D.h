#pragma once

#include <Core/Core.h>
#include <Camera/Camera.h>
#include <Math/Matrix4.h>
#include <Shape/Mesh.h>
#include <memory>
#include <vector>
#include "RenderPosition.h"


class UCameraComponent;
class ScreenBuffer;

class ENGINE_API Renderer3D
{
/* ------------------------------------------ Struct ------------------------------------------*/    
private:
    /* Frame은 실제 그리기와 무관하고, 모든 글자들의 문자, 색상, 정렬구조를 담고있는 정보집합 */
    struct Frame
    {
        Frame(int bufferCount);
        
        void Clear(const RenderPosition& screenSize);

        std::unique_ptr<CHAR_INFO[]> charInfoArray;
        std::unique_ptr<float[]> depthBuffer;
    };

    /* 화면상의 좌표값과 깊이값 */
    struct ScreenVertex
    {
        float x = 0.0f;
        float y = 0.0f;
        float inverseDepth = 0.0f;
    };

    /* 액터가 드로우 정보를 담아 요청 */
    struct RenderCommand
    {
        const Mesh* mesh = nullptr;
        Matrix4 worldMatrix;
    };

    
    
/* ------------------------------------------ Struct ------------------------------------------*/   
public:
    Renderer3D(const RenderPosition& screenSize);
    ~Renderer3D();
    
    static Renderer3D& Get();
    
/* Draw Console */    
public:
    void Submit(const Mesh& mesh, const Matrix4& worldMatrix);
    void Draw();

private:
    void Clear();
    void DrawRenderQueue();
    void Present();
    
    ScreenBuffer* GetCurrentBuffer();

/* Draw Triangle */    
private:
    void DrawTriangle(const ScreenVertex& v0,const ScreenVertex& v1,const ScreenVertex& v2,char character);

    static float Edge(const ScreenVertex& start,const ScreenVertex& end,float x,float y);
    
    void SetPixel(int x,int y,float inverseDepth,char character);    
    
    static char GetShadeCharacter(float brightness);

    ScreenVertex Project(const Vector3& viewPosition) const;
    
    
/* Camera */    
public:
    void SetCamera(const std::shared_ptr<UCameraComponent>& newCamera) { cameraComponent = newCamera; }

    
private:
    static Renderer3D* instance;
    
private:
    std::vector<RenderCommand> renderQueue;
    RenderPosition screenSize;
    std::unique_ptr<Frame> frame;
    std::unique_ptr<ScreenBuffer> screenBufferArray[2];
    int currentBufferIndex = 0;
    
    std::shared_ptr<UCameraComponent> cameraComponent;
    float focalLength = 1.0f;
};


