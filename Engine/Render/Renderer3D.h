#pragma once

#include <Core/Core.h>
#include <Math/Matrix4.h>
#include <Shape/Mesh.h>
#include <memory>
#include <string>
#include <vector>
#include "RenderPosition.h"
#include "Math/Color.h"


class UWorld;
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
    struct RenderCommand3D
    {
        const Mesh* mesh = nullptr;
        Matrix4 worldMatrix;
    };
    
    /* UI 전용 렌더 커맨드 */
    struct RenderCommand2D
    {
        std::string image;         
        RenderPosition position = RenderPosition(0,0);          
        Color color = Color::White;
    };

    
    
/* ------------------------------------------ Struct ------------------------------------------*/   
public:
    Renderer3D(const RenderPosition& screenSize);
    ~Renderer3D();
    
    static Renderer3D& Get();
    
/* Draw Console */    
public:
    void Submit(const Mesh& mesh, const Matrix4& worldMatrix);
    void SubmitUI(const std::string& image, const RenderPosition& position, Color color = Color::White); 
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
    
    
/* World for Camera */    
public:
    void SetWorld(const std::shared_ptr<UWorld>& newWorld) { world = newWorld; }
    
private:
    static Renderer3D* instance;
    
private:
    std::vector<RenderCommand3D> renderQueue;
    RenderPosition screenSize;
    std::unique_ptr<Frame> frame;
    std::unique_ptr<ScreenBuffer> screenBufferArray[2];
    int currentBufferIndex = 0; 
    
    std::vector<RenderCommand2D> renderQueueUI;
    std::weak_ptr<UWorld> world;    
    float focalLength = 1.0f;
};


