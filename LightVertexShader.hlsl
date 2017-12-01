#include "ConstantBuffers.hlsli"

float4x4 ProjView;
float4 MasterColor;

LightPixelShaderInput main(LightVertexShaderInput input)
{
    LightPixelShaderInput output = (LightPixelShaderInput)0;

    output.Position = mul(ProjView, input.Position);
    output.Normal = normalize(mul(ProjView, input.Normal));
    output.Color = MasterColor;

    return output;
}