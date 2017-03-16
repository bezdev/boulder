#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Mesh.h"
#include "Material.h"

class Model
{
public:
    Model(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, Mesh* mesh, Material* material);
    ~Model();

    DirectX::XMFLOAT3 GetPosition();
    DirectX::XMFLOAT4 GetRotation();

    Mesh* GetMesh();
    Material* GetMaterial();
protected:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_rotation;

    Mesh* m_mesh;
    Material* m_material;
};