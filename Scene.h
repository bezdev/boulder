#pragma once

#include <memory>
#include <vector>

#include "Model.h"

class Scene
{
public:
    Scene();
    void Render(DirectX::XMMATRIX* projView);
    void AddModel(Model* model);
protected:
    std::vector<std::unique_ptr<Model>> m_models;
};