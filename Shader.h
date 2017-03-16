#pragma once

#include <wrl.h>
#include <d3d11.h>

class Shader
{
public:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};

namespace Shaders
{
    extern Shader* SolidColorShader;
}