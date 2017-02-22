#include "Scene.h"

using namespace DirectX;

Scene::Scene()
{
    m_models.clear();
}

void Scene::Render(XMMATRIX* projView)
{
    for (std::size_t i = 0; i < m_models.size(); i++)
    {
        m_models[i]->Render(projView);
    }
}

void Scene::AddModel(Model* model)
{
    m_models.push_back(std::unique_ptr<Model>(model));
}