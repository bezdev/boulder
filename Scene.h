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
    std::vector<std::unique_ptr<Model>> m_models;
};

class SceneManager
{
public:
    static Scene* CreateColorCubeScene();
    static Scene* CreateNormalCubeScene();
private:
    SceneManager();
};