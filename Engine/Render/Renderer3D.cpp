#define NOMINMAX

#include "Renderer3D.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include <Windows.h>

#include "ScreenBuffer.h"
#include "Component/CameraComponent.h"
#include "World/World.h"


// -------------------------- Frame -------------------------- //

Renderer3D::Frame::Frame(int bufferCount)
{
    charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
    depthBuffer = std::make_unique<float[]>(bufferCount);
}

void Renderer3D::Frame::Clear(const RenderPosition& screenSize)
{
    const int bufferCount = screenSize.x * screenSize.y;

    for (int index = 0; index < bufferCount; ++index)
    {
        charInfoArray[index].Char.AsciiChar = ' ';
        charInfoArray[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        depthBuffer[index] = 0.0f; // depth값이 클수록 카메라와 가까움
    }
}


// -------------------------- Renderer -------------------------- //

Renderer3D* Renderer3D::instance = nullptr;

Renderer3D::Renderer3D(const RenderPosition& screenSize)
    : screenSize(screenSize)    
{
    assert(!instance && "Renderer3D instance already exists");
    instance = this;

    /* Frame 생성 */
    const int bufferCount = screenSize.x * screenSize.y;
    frame = std::make_unique<Frame>(bufferCount);
    frame->Clear(screenSize);

    /* Double Buffer 생성 */
    screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
    screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);

    screenBufferArray[0]->Clear();
    screenBufferArray[1]->Clear();

    SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetBuffer());

    /* Perspective Projection 설정 */
    constexpr float pi = 3.14159265359f;
    const float fieldOfView = 45.0f * pi / 180.0f;

    focalLength = (static_cast<float>(screenSize.y) * 0.5f) / std::tan(fieldOfView * 0.5f);
}

Renderer3D::~Renderer3D()
{
    instance = nullptr;

    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
}

Renderer3D& Renderer3D::Get()
{
    assert(instance && "Renderer3D instance is null");
    return *instance;
}


void Renderer3D::Submit(const Mesh& mesh, const Matrix4& worldMatrix)
{
    RenderCommand command;
    command.mesh = &mesh;
    command.worldMatrix = worldMatrix;

    renderQueue.emplace_back(command);
}

void Renderer3D::Draw()
{
    Clear();
    DrawRenderQueue();
    Present();
}

void Renderer3D::Clear()
{
    frame->Clear(screenSize);
    GetCurrentBuffer()->Clear();
}

void Renderer3D::DrawRenderQueue()
{
    const Vector3 lightDirection = Vector3{-0.4f, 1.0f, -0.6f}.Normalized();
    
    const std::shared_ptr<UWorld> currentWorld = world.lock();
    if (!currentWorld) return;
    const std::shared_ptr<UCameraComponent> camera = currentWorld->GetActiveCamera();
    if (!camera) return;

    const Matrix4 viewMatrix = camera->GetViewMatrix();        

    for (const RenderCommand& command : renderQueue)
    {
        if (command.mesh == nullptr) continue;

        const Mesh& mesh = *command.mesh;

        /* World Matrix 좌표계 변환 */
        const std::vector<Vector3>& vertices = mesh.GetVertices();
        std::vector<Vector3> worldVertices;
        worldVertices.reserve(vertices.size());

        for (const Vector3& vertex : vertices)
        {
            const Vector3 worldVertex = vertex * command.worldMatrix;
            worldVertices.emplace_back(worldVertex);
        }

        /* World 좌표계 기준 버텍스들 좌표계변환(World->Camera->Projection) 후 Draw 처리 */
        for (const TriangleIndex& triangle : mesh.GetTriangles())
        {
            const Vector3& world0 = worldVertices[triangle.indices[0]];
            const Vector3& world1 = worldVertices[triangle.indices[1]];
            const Vector3& world2 = worldVertices[triangle.indices[2]];

            /* View Matrix 좌표계변환 */
            const Vector3 view0 = world0 * viewMatrix;
            const Vector3 view1 = world1 * viewMatrix;
            const Vector3 view2 = world2 * viewMatrix;

            /* Near Plane 컬링 */
            constexpr float nearPlane = 0.1f;
            if (view0.z <= nearPlane || view1.z <= nearPlane || view2.z <= nearPlane) continue;

            /* Back 컬링 */
            const Vector3 viewNormal = (view1 - view0).Cross(view2 - view0).Normalized();
            if (viewNormal.z >= 0.0f) continue;

            /* 광원 처리로 문자열 설정 */
            const Vector3 worldNormal = (world1 - world0).Cross(world2 - world0).Normalized();
            const float diffuse = std::max(0.0f, worldNormal.Dot(lightDirection));
            constexpr float ambientLight = 0.2f;
            const float brightness = ambientLight + (1.0f - ambientLight) * diffuse;

            /* Projection Matrix 좌표계 변환 */
            const ScreenVertex screen0 = Project(view0);
            const ScreenVertex screen1 = Project(view1);
            const ScreenVertex screen2 = Project(view2);

            /* Rasterize */
            DrawTriangle(screen0, screen1, screen2, GetShadeCharacter(brightness));
        }
    }


    /* 완성된 Frame 전체를 ScreenBuffer에 전달 */
    GetCurrentBuffer()->Draw(frame->charInfoArray.get());

    /* 이번 프레임 RenderCommand 제거 */
    renderQueue.clear();
}

Renderer3D::ScreenVertex Renderer3D::Project(const Vector3& viewPosition) const
{
    const float projectedX = (viewPosition.x / viewPosition.z) * focalLength;
    const float projectedY = (viewPosition.y / viewPosition.z) * focalLength;

    return
    {
        static_cast<float>(screenSize.x) * 0.5f + projectedX * 2.0f,
        static_cast<float>(screenSize.y) * 0.5f - projectedY,
        1.0f / viewPosition.z
    };
}


void Renderer3D::DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2,char character)
{
    /* 삼각형을 완전히 감싸는 최소 Bounding Box */
    const int minX = std::max(0, static_cast<int>(std::floor(std::min({v0.x, v1.x, v2.x}))));
    const int maxX = std::min(screenSize.x - 1, static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
    const int minY = std::max(0, static_cast<int>(std::floor(std::min({v0.y, v1.y, v2.y}))));
    const int maxY = std::min(screenSize.y - 1, static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

    const float area = Edge(v0, v1, v2.x, v2.y);

    if (std::abs(area) < 0.00001f) return;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            const float pixelX = static_cast<float>(x) + 0.5f;
            const float pixelY = static_cast<float>(y) + 0.5f;

            /* Barycentric Coordinate */
            const float weight0 = Edge(v1, v2, pixelX, pixelY) / area;
            const float weight1 = Edge(v2, v0, pixelX, pixelY) / area;
            const float weight2 = Edge(v0, v1, pixelX, pixelY) / area;

            /* Triangle 외부 */
            if (weight0 < 0.0f || weight1 < 0.0f || weight2 < 0.0f) continue;

            /* 세 Vertex의 깊이를 보간 */
            const float inverseDepth =
                weight0 * v0.inverseDepth +
                weight1 * v1.inverseDepth +
                weight2 * v2.inverseDepth;

            SetPixel(x, y, inverseDepth, character);
        }
    }
}

void Renderer3D::SetPixel(int x, int y, float inverseDepth, char character)
{
    if (x < 0 || x >= screenSize.x) return;
    if (y < 0 || y >= screenSize.y) return;

    const int index = y * screenSize.x + x;

    if (inverseDepth <= frame->depthBuffer[index]) return;

    frame->depthBuffer[index] = inverseDepth;

    frame->charInfoArray[index].Char.AsciiChar = character;
    frame->charInfoArray[index].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
}


float Renderer3D::Edge(
    const ScreenVertex& start,
    const ScreenVertex& end,
    float x,
    float y)
{
    return (x - start.x) * (end.y - start.y) - (y - start.y) * (end.x - start.x);
}


char Renderer3D::GetShadeCharacter(float brightness)
{
    static const std::string shadeCharacters = ".:-=+*#%@";

    brightness = std::clamp(brightness, 0.0f, 1.0f);

    const int index =
        static_cast<int>(brightness * static_cast<float>(shadeCharacters.size() - 1));

    return shadeCharacters[index];
}

void Renderer3D::Present()
{
    SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

    currentBufferIndex = 1 - currentBufferIndex;
}

ScreenBuffer* Renderer3D::GetCurrentBuffer()
{
    return screenBufferArray[currentBufferIndex].get();
}