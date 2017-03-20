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
    ID3D11Device* d3dDevice = gOculusApp->GetRenderer()->GetDevice();

    std::vector<char> compiledVertexShader = FileReader::ReadCSO(vertexShaderFileName);
    THROW_IF_FAILED(d3dDevice->CreateVertexShader(compiledVertexShader.data(), compiledVertexShader.size(), nullptr, &VertexShader), "CreateVertexShader failed.");

    THROW_IF_FAILED(d3dDevice->CreateInputLayout(
        inputDesc,
        numInputElements,
        compiledVertexShader.data(),
        compiledVertexShader.size(),
        &InputLayout),
        "CreateInputLayout failed.");

    std::vector<char> compiledPixelShader = FileReader::ReadCSO(pixelShaderFileName);
    THROW_IF_FAILED(d3dDevice->CreatePixelShader(compiledPixelShader.data(), compiledPixelShader.size(), nullptr, &PixelShader), "CreatePixelShader failed.");
}

ColorShader::ColorShader() :
    Shader("VertexColorShader.cso",
           "PixelColorShader.cso",
           gColorVertexInputDesc,
           sizeof(gColorVertexInputDesc) / sizeof(*gColorVertexInputDesc))
{
}
