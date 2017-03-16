#pragma once

#include "Shader.h"

class Material
{
public:
    Material(Shader* shader);
    Shader* GetShader();
protected:
    Shader* m_shader;
};

namespace Materials
{
    extern Material* SolidColorMaterial;
}

class SolidColorMaterial : public Material
{
public:
    SolidColorMaterial();
};