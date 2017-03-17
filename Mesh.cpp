#include "Mesh.h"

namespace Meshes
{
    namespace Primitives
    {
        Mesh* BoxMesh;
        Mesh* AxisMesh;
    }
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
    return m_vertexBuffer.Get();
}

ID3D11Buffer* Mesh::GetIndexBuffer()
{
    return m_indexBuffer.Get();
}

UINT Mesh::GetVertexCount()
{
    return m_vertexCount;
}

UINT Mesh::GetIndexCount()
{
    return m_indexCount;
}

size_t* Mesh::GetVertexSize()
{
    return &m_vertexSize;
}

D3D_PRIMITIVE_TOPOLOGY Mesh::GetTopology()
{
    return m_topology;
}

MeshType Mesh::GetType()
{
    return m_meshType;
}
