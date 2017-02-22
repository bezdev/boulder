#pragma once

#include <DirectXMath.h>

namespace ConstantBuffers
{
    __declspec(align(16)) struct ChangesEveryPrim
    {
        DirectX::XMMATRIX WorldMatrix;
        DirectX::XMFLOAT4 Color;
    };
}
