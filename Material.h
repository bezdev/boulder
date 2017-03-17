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

class SolidColorMaterial : public Material
{
public:
    SolidColorMaterial();
    SolidColorMaterial(DirectX::XMFLOAT4 color);

    MaterialType GetType();
    DirectX::XMFLOAT4 GetColor();
protected:
    DirectX::XMFLOAT4 m_color;
};