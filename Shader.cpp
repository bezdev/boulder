#include <d3d11.h>
#include <vector>

#include "Shader.h"
#include "FileReader.h"
#include "OculusApp.h"

namespace Shaders
{
    Shader* ColorShader;
}

D3D11_INPUT_ELEMENT_DESC gColorVertexInputDesc[] = {
    { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "Color",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

Shader::Shader(const char * vertexShaderFileName,
               const char * pixelShaderFileName,
               D3D11_INPUT_ELEMENT_DESC* inputDesc,
               UINT numInputElements)
{
    std::vector<char> compiledVertexShader = FileReader::ReadCSO(vertexShaderFileName);
    THROW_IF_FAILED(DIRECTX::Device->CreateVertexShader(compiledVertexShader.data(), compiledVertexShader.size(), nullptr, &VertexShader), "CreateVertexShader failed.");

    THROW_IF_FAILED(DIRECTX::Device->CreateInputLayout(
        inputDesc,
        numInputElements,
        compiledVertexShader.data(),
        compiledVertexShader.size(),
        &InputLayout),
        "CreateInputLayout failed.");

    std::vector<char> compiledPixelShader = FileReader::ReadCSO(pixelShaderFileName);
    THROW_IF_FAILED(DIRECTX::Device->CreatePixelShader(compiledPixelShader.data(), compiledPixelShader.size(), nullptr, &PixelShader), "CreatePixelShader failed.");
}

void Shader::Set()
{
    DIRECTX::DeviceContext->IASetInputLayout(InputLayout.Get());
    DIRECTX::DeviceContext->VSSetShader(VertexShader.Get(), nullptr, 0);
    DIRECTX::DeviceContext->PSSetShader(PixelShader.Get(), nullptr, 0);
}

ColorShader::ColorShader() :
    Shader("ColorVertexShader.cso",
           "ColorPixelShader.cso",
           gColorVertexInputDesc,
           sizeof(gColorVertexInputDesc) / sizeof(*gColorVertexInputDesc))
{
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(ChangesEveryPrim);
    THROW_IF_FAILED(DIRECTX::Device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer), "Create Buffer failed.");
}

void ColorShader::Set()
{
    Shader::Set();
    DIRECTX::DeviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
}

void XM_CALLCONV ColorShader::UpdateConstantBuffer(DirectX::XMMATRIX world, DirectX::XMFLOAT4 color)
{
    ChangesEveryPrim changesEveryPrim;
    changesEveryPrim.worldMatrix = world;
    changesEveryPrim.color = color;

    D3D11_MAPPED_SUBRESOURCE map;
    THROW_IF_FAILED(DIRECTX::DeviceContext->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map), "Map failed.");
    memcpy(map.pData, &changesEveryPrim, sizeof(changesEveryPrim));
    DIRECTX::DeviceContext->Unmap(m_constantBuffer.Get(), 3);
}
