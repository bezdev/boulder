#include "bez.h"

#include "Scene.h"
#include "Mesh.h"
#include "Material.h"

using namespace DirectX;

Scene::Scene()
{
    m_models.clear();
}

void Scene::AddModel(Model* model)
{
    m_models.push_back(std::unique_ptr<Model>(model));
}

Scene* SceneManager::CreateColorCubeScene()
{
    const int NUM_CUBES = 100;
    const float BOUNDS = 10.f;

    Scene* scene = new Scene();

    for (int i = 0; i < NUM_CUBES; i++)
    {
        float x = RandomInRange(-BOUNDS, BOUNDS);
        float y = RandomInRange(-BOUNDS, BOUNDS);
        float z = RandomInRange(-BOUNDS, BOUNDS);

        XMFLOAT4 rot;
        float roll = RandomInRange(0, XM_2PI);
        float pitch = RandomInRange(0, XM_2PI);
        float yaw = RandomInRange(0, XM_2PI);
        XMStoreFloat4(&rot, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

        DirectX::XMFLOAT4 color;
        int colorIndex = static_cast<int>(round(RandomInRange(1, 3)));
        switch (colorIndex)
        {
        case 1:
            color = Colors::Red;
            break;
        case 2:
            color = Colors::Green;
            break;
        case 3:
            color = Colors::Blue;
            break;
        }

        scene->AddModel(new Model(XMFLOAT3(x, y, z), rot, Meshes::Primitives::BoxColorMesh, new ColorMaterial(color)));
    }

    scene->AddModel(new Model(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), Meshes::Primitives::AxisMesh, new ColorMaterial()));

    return scene;
}

Scene* SceneManager::CreateNormalCubeScene()
{
    Scene* scene = new Scene();

    scene->AddModel(new Model(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), Meshes::Primitives::BoxColorMesh, new ColorMaterial(Colors::Red)));
    scene->AddModel(new Model(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), Meshes::Primitives::AxisMesh, new ColorMaterial()));

    return scene;
}
