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
    return m_materialType;
}

ColorMaterial::ColorMaterial() :
    Material(Shaders::ColorShader, MaterialType::SolidColor),
    m_color(Colors::White)
{
}

ColorMaterial::ColorMaterial(DirectX::XMFLOAT4 color) :
    Material(Shaders::ColorShader, MaterialType::SolidColor),
    m_color(color)
{
}

ColorShader * ColorMaterial::GetShader()
{
    return (ColorShader*)m_shader;
}

MaterialType ColorMaterial::GetType()
{
    return m_materialType;
}

DirectX::XMFLOAT4 ColorMaterial::GetColor()
{
    return m_color;
}
