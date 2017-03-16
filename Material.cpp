#include "Material.h"
#include "Renderer.h"

namespace Materials
{
    Material* SolidColorMaterial;
}

Material::Material(Shader* shader):
    m_shader(shader)
{
}

Shader* Material::GetShader()
{
    return m_shader;
}

SolidColorMaterial::SolidColorMaterial():
    Material(Shaders::SolidColorShader)
{
}
