#include "Material.h"
#include "Renderer.h"

Material::Material(Shader* shader, MaterialType materialType):
    m_shader(shader),
    m_materialType(materialType)
{
}

Shader* Material::GetShader()
{
    return m_shader;
}

MaterialType Material::GetType()
{
    return MaterialType();
}

SolidColorMaterial::SolidColorMaterial() :
    Material(Shaders::SolidColorShader, MaterialType::SolidColor),
    m_color(Colors::White)
{
}

SolidColorMaterial::SolidColorMaterial(DirectX::XMFLOAT4 color) :
    Material(Shaders::SolidColorShader, MaterialType::SolidColor),
    m_color(color)
{
}

MaterialType SolidColorMaterial::GetType()
{
    return m_materialType;
}

DirectX::XMFLOAT4 SolidColorMaterial::GetColor()
{
    return m_color;
}
