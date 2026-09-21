#pragma once
#include <array>
#include <vector>
#include <Core/Core.h>
#include <Math/Vector3.h>

struct TriangleIndex
{
    std::array<int, 3> indices;
};

class ENGINE_API Mesh
{
public:
    static Mesh CreateCube();

    const std::vector<Vector3>& GetVertices() const { return vertices; }
    const std::vector<TriangleIndex>& GetTriangles() const { return triangles; }

private:
    std::vector<Vector3> vertices;
    std::vector<TriangleIndex> triangles;
};
