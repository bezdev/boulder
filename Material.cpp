#include "Material.h"
#include "Renderer.h"

Material::Material(Shader* shader):
    m_shader(shader)
{
}

Material::~Material()
{
}

Shader* Material::GetShader()
{
    return m_shader;
}

SolidColorMaterial::SolidColorMaterial() :
    Material(Shaders::SolidColorShader),
    m_color(Colors::White)
{
}

SolidColorMaterial::SolidColorMaterial(DirectX::XMFLOAT4 color):
    Material(Shaders::SolidColorShader),
    m_color(color)
{
}

DirectX::XMFLOAT4 SolidColorMaterial::GetColor()
{
    return m_color;
}
