#include "bez.h"

#include "Model.h"
#include "OculusApp.h"

Model::Model(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, Mesh* mesh, Material* material) :
    m_position(position),
    m_rotation(rotation),
    m_mesh(mesh),
    m_material(material)
{
    gOculusApp->GetRenderer()->AddRenderObject(this);
}

Model::~Model()
{
    delete m_material;
    m_material = nullptr;
}

DirectX::XMFLOAT3 Model::GetPosition()
{
    return m_position;
}

DirectX::XMFLOAT4 Model::GetRotation()
{
    return m_rotation;
}

Mesh* Model::GetMesh()
{
    return m_mesh;
}

Material* Model::GetMaterial()
{
    return m_material;
}
