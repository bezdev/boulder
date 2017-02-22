#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMVECTOR* position, XMVECTOR* rotation):
    m_position(*position),
    m_rotation(*rotation)
{
}

Camera::Camera(const XMVECTOR& position, const XMVECTOR& rotation):
    m_position(position),
    m_rotation(rotation)
{
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), m_rotation);
    return (XMMatrixLookAtRH(m_position, XMVectorAdd(m_position, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), m_rotation)));
}