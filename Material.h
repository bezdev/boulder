#pragma once

#include <DirectXMath.h>

#include "Shader.h"

class Material
{
public:
    Material(Shader* shader);
    virtual ~Material();
    Shader* GetShader();
protected:
    Shader* m_shader;
};

class SolidColorMaterial : public Material
{
public:
    SolidColorMaterial();
    SolidColorMaterial(DirectX::XMFLOAT4 color);

    DirectX::XMFLOAT4 GetColor();
protected:
    DirectX::XMFLOAT4 m_color;
};