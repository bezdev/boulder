#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Shader
{
public:
    Shader(const char * vertexShaderFileName,
           const char * pixelShaderFileName,
           D3D11_INPUT_ELEMENT_DESC* inputDesc,
           UINT numInputElements);

    virtual void Set();

    Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
};

class ColorShader : public Shader
{
public:
    __declspec(align(16)) struct ChangesEveryPrim
    {
        DirectX::XMMATRIX worldMatrix;
        DirectX::XMFLOAT4 color;
    };

    ColorShader();

    void Set();

    void XM_CALLCONV UpdateConstantBuffer(DirectX::XMMATRIX world, DirectX::XMFLOAT4 color);
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
};

namespace Shaders
{
    extern Shader* ColorShader;
}