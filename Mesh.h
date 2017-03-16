#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

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

struct ColorVertex
{
    ColorVertex::ColorVertex(float x, float y, float z, DirectX::XMFLOAT4 color) :
        Position(DirectX::XMFLOAT3(x, y, z)),
        Color(color)
    {
    }

    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT4 Color;
};

template <typename T>
struct MeshData
{
    std::vector<T> Vertices;
    std::vector<unsigned short> Indices;
    D3D_PRIMITIVE_TOPOLOGY Topology;
};

namespace GeometryBuilder
{
    static void CreateBoxMesh(float width, float height, float depth, DirectX::XMFLOAT4 color, MeshData<ColorVertex>& meshData)
    {
        float x2 = 0.5f * width;
        float y2 = 0.5f * height;
        float z2 = 0.5f * depth;

        meshData.Vertices.assign({
            ColorVertex(-x2, -y2, -z2, color),
            ColorVertex(-x2, +y2, -z2, color),
            ColorVertex(+x2, +y2, -z2, color),
            ColorVertex(+x2, -y2, -z2, color),
            ColorVertex(-x2, -y2, +z2, color),
            ColorVertex(-x2, +y2, +z2, color),
            ColorVertex(+x2, +y2, +z2, color),
            ColorVertex(+x2, -y2, +z2, color)
        });

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
            4, 3, 7
        });

        meshData.Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    static void CreateAxisMesh(float x, float y, float z, MeshData<ColorVertex>& meshData)
    {
        meshData.Vertices.assign({
            ColorVertex(0, 0, 0, Colors::Red),
            ColorVertex(x, 0, 0, Colors::Red),
            ColorVertex(0, 0, 0, Colors::Blue),
            ColorVertex(0, y, 0, Colors::Blue),
            ColorVertex(0, 0, 0, Colors::Green),
            ColorVertex(0, 0, z, Colors::Green)
        });

        meshData.Indices.assign({
            0, 1, // x
            2, 3, // y
            4, 5  // z
        });

        meshData.Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    }
}

class Mesh
{
public:
    template <class T>
    Mesh(ID3D11Device* device, MeshData<T>& meshData);

    ID3D11Buffer* GetVertexBuffer();
    ID3D11Buffer* GetIndexBuffer();
    UINT GetVertexCount();
    UINT GetIndexCount();
    size_t* GetVertexSize();
    D3D_PRIMITIVE_TOPOLOGY GetTopology();
protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_vertexCount;
    UINT m_indexCount;
    size_t m_vertexSize;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
};

template<typename T>
inline Mesh::Mesh(ID3D11Device* device, MeshData<T>& meshData) :
    m_vertexCount(meshData.Vertices.size()),
    m_indexCount(meshData.Indices.size()),
    m_topology(meshData.Topology)
{
    m_vertexSize = sizeof(T);
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(T) * m_vertexCount;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &meshData.Vertices[0];
    THROW_IF_FAILED(device->CreateBuffer(&bd, &initData, &m_vertexBuffer), "CreateBuffer failed.");

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(short) * m_indexCount;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    initData.pSysMem = &meshData.Indices[0];
    THROW_IF_FAILED(device->CreateBuffer(&bd, &initData, &m_indexBuffer), "CreateBuffer failed.");
}

namespace Meshes
{
    namespace Primitives
    {
        extern Mesh* BoxMesh;
        extern Mesh* AxisMesh;
    }
}
