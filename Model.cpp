#include "bez.h"

#include "Model.h"
#include "OculusApp.h"
#include "ConstantBuffers.h"

using namespace DirectX;


Model::Model(ID3D11Device* device, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, MeshData& meshData) :
    m_position(position),
    m_rotation(rotation),
    m_vertexCount(meshData.Vertices.size()),
    m_indexCount(meshData.Indices.size()),
    m_topology(meshData.Topology)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(Vertex) * m_vertexCount;
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

Model::~Model()
{
}

void Model::Render(XMMATRIX* projView)
{    
    ID3D11DeviceContext* deviceContext = gOculusApp->GetRenderer()->GetDeviceContext();

    XMMATRIX modelMat = XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation)), XMMatrixTranslationFromVector(XMLoadFloat3(&m_position)));
    ConstantBuffers::ChangesEveryPrim changesEveryPrim;
    changesEveryPrim.WorldMatrix = XMMatrixMultiply(modelMat, *projView);
    changesEveryPrim.Color = { 1.f, 1.f, 1.f, 1.f };

    ID3D11Buffer* constantBuffer = gOculusApp->GetRenderer()->GetConstantBuffer();
    D3D11_MAPPED_SUBRESOURCE map;
    THROW_IF_FAILED(deviceContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map), "Map failed.");
    memcpy(map.pData, &changesEveryPrim, sizeof(changesEveryPrim));
    deviceContext->Unmap(constantBuffer, 0);
    
    UINT offset = 0;
    UINT size = sizeof(Vertex);
    deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &size, &offset);
    deviceContext->IASetPrimitiveTopology(m_topology);
    deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    deviceContext->DrawIndexed(m_indexCount, 0, 0);
}
