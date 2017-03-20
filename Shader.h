#pragma once

#include <wrl.h>
#include <d3d11.h>

class Shader
{
public:
    Shader(const char * vertexShaderFileName,
           const char * pixelShaderFileName,
           D3D11_INPUT_ELEMENT_DESC* inputDesc,
           UINT numInputElements);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};

class ColorShader : public Shader
{
public:
    ColorShader();
};

namespace Shaders
{
    extern Shader* ColorShader;
}