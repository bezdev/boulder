#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

Scene::Scene()
{
    m_models.clear();
    InitializeScene();
}

void Scene::AddModel(Model* model)
{
    m_models.push_back(std::unique_ptr<Model>(model));
}

void Scene::InitializeScene()
{
    AddModel(new Model(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), Meshes::Primitives::BoxMesh, Materials::SolidColorMaterial));
    AddModel(new Model(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), Meshes::Primitives::AxisMesh, Materials::SolidColorMaterial));
}
