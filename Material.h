#pragma once

#include <DirectXMath.h>

#include "Shader.h"

enum MaterialType
{
    SolidColor
};

class Material
{
public:
    Material(Shader* shader, MaterialType materialType);
    Shader* GetShader();
    virtual MaterialType GetType() = 0;
protected:
    Shader* m_shader;
    MaterialType m_materialType;
};

class ColorMaterial : public Material
{
public:
    ColorMaterial();
    ColorMaterial(DirectX::XMFLOAT4 color);

    ColorShader* GetShader();
    MaterialType GetType();
    DirectX::XMFLOAT4 GetColor();
protected:
    DirectX::XMFLOAT4 m_color;
};