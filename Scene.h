#pragma once

#include <memory>
#include <vector>

#include "Model.h"

class Scene
{
public:
    Scene();
    void AddModel(Model* model);
protected:
    void InitializeScene();

    std::vector<std::unique_ptr<Model>> m_models;
};