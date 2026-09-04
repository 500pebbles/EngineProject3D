#include "Renderer3D.h"

#include <algorithm>
#include <cmath>
#include <string>

Renderer3D::Renderer3D(ConsoleScreen& screen)
    : _screen(screen), _camera(Vector3{3.0f, 2.0f, -4.0f},Vector3{0.0f, 0.0f, 0.0f})
{
    /* FOV값 설정 : 45 (radian 변환) */
    const float pi = 3.14159265359f;                 
    const float fieldOfView = 45.0f * pi / 180.0f;  

    /* 밑변길이 : 화면과 Camera 상대위치 */
    _focalLength =(static_cast<float>(_screen.GetHeight()) * 0.5f) / std::tan(fieldOfView * 0.5f);
}

void Renderer3D::Render(const Mesh& mesh, float rotationAngle)
{
    _screen.Clear();
        
    /* 원본 Vertex */
    const std::vector<Vector3>& vertices = mesh.GetVertices();
    
    /* 원본 Vertex는 유지하고 이번 프레임에서만 변형을 겪을 임시 벡터 */
    std::vector<Vector3> transformedVertices;
    transformedVertices.reserve(vertices.size());

    for (const Vector3& vertex : vertices)
    {
        /* 임시버텍스에 적용할 회전값 적용 */
        Vector3 transformed = vertex.RotateY(rotationAngle);
        transformed = transformed.RotateX(rotationAngle * 0.5f);

        /* 임시버텍스 배열에 값을 채움 */
        transformedVertices.emplace_back(transformed);
    }

    /* 가상의 광원의 방향 정규화 */
    const Vector3 lightDirection = Vector3{-0.4f, 1.0f, -0.6f}.Normalized();

    /* 메쉬가 보유한 모든 Triangle에 대해 */
    for (const TriangleIndex& triangle : mesh.GetTriangles())
    {
        /* 실제 월드에서의 버텍스 */
        const Vector3& world0 = transformedVertices[triangle.indices[0]];
        const Vector3& world1 = transformedVertices[triangle.indices[1]];
        const Vector3& world2 = transformedVertices[triangle.indices[2]];

        /* 카메라 좌표계로 변환된 버텍스 */
        const Vector3 view0 = _camera.WorldToView(world0);
        const Vector3 view1 = _camera.WorldToView(world1);
        const Vector3 view2 = _camera.WorldToView(world2);

        /* 지나치게 가까운 범위는 렌더링하지 않도록 nearPlane을 설정 */
        constexpr float nearPlane = 0.1f;
        if (view0.z <= nearPlane || view1.z <= nearPlane || view2.z <= nearPlane)  continue;

        /* 렌더링 여부를 위해 카메라 좌표계 기준으로 viewNormal을 구함 */
        const Vector3 viewNormal = (view1 - view0).Cross(view2 - view0).Normalized();
        if (viewNormal.z >= 0.0f) continue;
        
        /* 빛 연산을 위해 월드 좌표계 기준으로 worldNormal을 구함 */
        const Vector3 worldNormal = (world1 - world0).Cross(world2 - world0).Normalized();

        /* worldNormal과 lightDirection 벡터방향이 같을수록 (= 1에 가까울수록) 빛연산 가중치 */
        const float diffuse = std::max(0.0f,worldNormal.Dot(lightDirection));

        /* 최소밝기값 하한 설정 */
        constexpr float ambientLight = 0.2f;

        /* 최종 밝기값 */
        const float brightness = ambientLight +(1.0f - ambientLight) * diffuse;

        /* DrawTriangle과 Project라는 함수를 사용하는데, 이건 읽기전까지는 모름 */        
        DrawTriangle(Project(view0),Project(view1),Project(view2),GetShadeCharacter(brightness));
    }

    _screen.Present();
}

Renderer3D::ScreenVertex Renderer3D::Project(const Vector3& viewPosition) const
{
    /* 카메라좌표계로 변환된 삼각형의 정점위치를 변환 */
    const float projectedX = (viewPosition.x / viewPosition.z) * _focalLength;
    const float projectedY = (viewPosition.y / viewPosition.z) * _focalLength;

    /* 최종적으로 3D데이터를 2D터미널 화면으로 변환 */
    return
    {
        static_cast<float>(_screen.GetWidth()) * 0.5f + projectedX * 2.0f,
        static_cast<float>(_screen.GetHeight()) * 0.5f + projectedY * -1.f,
        1.0f / viewPosition.z
    };
}

void Renderer3D::DrawTriangle(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2, char character)
{
    const int minX = std::max(0, static_cast<int>(std::floor(std::min({v0.x, v1.x, v2.x}))));
    const int maxX = std::min(_screen.GetWidth() - 1,static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x}))));
    const int minY = std::max(0,static_cast<int>(std::floor(std::min({v0.y, v1.y, v2.y}))));
    const int maxY = std::min(_screen.GetHeight() - 1,static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y}))));

    const float area = Edge(v0, v1, v2.x, v2.y);

    /* 넓이가 0이면 삼각형이 아니므로 생략 */
    if (std::abs(area) < 0.00001f) return;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        { 
            /* 콘솔 칸의 중앙 좌표를 검사 */
            const float pixelX = static_cast<float>(x) + 0.5f;
            const float pixelY = static_cast<float>(y) + 0.5f;

            /* 세 변에 대한 위치를 이용해 무게중심 좌표를 구한다 */
            const float weight0 = Edge(v1, v2, pixelX, pixelY) / area;
            const float weight1 = Edge(v2, v0, pixelX, pixelY) / area;
            const float weight2 = Edge(v0, v1, pixelX, pixelY) / area;

            /* 세 가중치가 모두 0 이상이면 픽셀이 삼각형 내부에 있다 */
            if (weight0 < 0.0f || weight1 < 0.0f || weight2 < 0.0f) continue;
            
            /* 세 정점의 깊이를 보간해 현재 픽셀의 깊이를 구한다 */
            const float inverseDepth = weight0 * v0.inverseDepth + weight1 * v1.inverseDepth + weight2 * v2.inverseDepth;

            /* 드로우 */
            _screen.SetPixel(x, y, inverseDepth, character);
        }
    }
}

float Renderer3D::Edge(const ScreenVertex& start, const ScreenVertex& end, float x, float y)
{
    return (x - start.x) * (end.y - start.y) - (y - start.y) * (end.x - start.x);
}

char Renderer3D::GetShadeCharacter(float brightness)
{
    static const std::string shadeCharacters = ".:-=+*#%@";

    brightness = std::clamp(brightness, 0.0f, 1.0f);        
    const int index = static_cast<int>(brightness * static_cast<float>(shadeCharacters.size() - 1));
    // if문으로 변경하면 정확하게 나눠진 범위말고 중간값을 늘릴수 있을듯

    return shadeCharacters[index];
}