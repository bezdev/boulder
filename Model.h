#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>

#include "Mesh.h"

class Model
{
public:
    Model(ID3D11Device* device, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, MeshData& meshData);
    ~Model();

    void Render(DirectX::XMMATRIX* projView);
protected:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_rotation;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_vertexCount;
    UINT m_indexCount;
};
