#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <vector>

namespace Colors
{
    XMGLOBALCONST DirectX::XMFLOAT4 White(1.0f, 1.0f, 1.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Black(0.0f, 0.0f, 0.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Red(1.0f, 0.0f, 0.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Green(0.0f, 1.0f, 0.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Blue(0.0f, 0.0f, 1.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Yellow(1.0f, 1.0f, 0.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Cyan(0.0f, 1.0f, 1.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Magenta(1.0f, 0.0f, 1.0f, 1.0f);
    XMGLOBALCONST DirectX::XMFLOAT4 Silver(0.75f, 0.75f, 0.75f, 1.0f);
}

struct Vertex
{
    Vertex::Vertex(float x, float y, float z, DirectX::XMFLOAT4 color) :
        Position(DirectX::XMFLOAT3(x, y, z)),
        Color(color)
    {
    }

    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT4 Color;
};

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<short> Indices;
};

namespace
{
    void CreateBoxMesh(float width, float height, float depth, MeshData& meshData)
    {
        float x2 = 0.5f * width;
        float y2 = 0.5f * height;
        float z2 = 0.5f * depth;

        meshData.Vertices.assign({
            Vertex(-x2, -y2, -z2, Colors::Green),
            Vertex(-x2, +y2, -z2, Colors::Green),
            Vertex(+x2, +y2, -z2, Colors::Green),
            Vertex(+x2, -y2, -z2, Colors::Green),
            Vertex(-x2, -y2, +z2, Colors::Green),
            Vertex(-x2, +y2, +z2, Colors::Green),
            Vertex(+x2, +y2, +z2, Colors::Green),
            Vertex(+x2, -y2, +z2, Colors::Green) });

        meshData.Indices.assign({
            // front face
            0, 1, 2,
            0, 2, 3,
            // back face
            4, 6, 5,
            4, 7, 6,
            // left face
            4, 5, 1,
            4, 1, 0,
            // right face
            3, 2, 6,
            3, 6, 7,
            // top face
            1, 5, 6,
            1, 6, 2,
            // bottom face
            4, 0, 3,
            4, 3, 7 });
    }
}