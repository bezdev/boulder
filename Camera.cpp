#include "Camera.h"

using namespace DirectX;

Camera::Camera(XMVECTOR* position, XMVECTOR* rotation):
    m_position(*position),
    m_rotation(*rotation),
    m_roll(0.f),
    m_pitch(0.f),
    m_yaw(0.f)
{
}

Camera::Camera(const XMVECTOR& position, const XMVECTOR& rotation):
    m_position(position),
    m_rotation(rotation),
    m_roll(0.f),
    m_pitch(0.f),
    m_yaw(0.f)
{
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), m_rotation);
    return (XMMatrixLookAtRH(m_position, XMVectorAdd(m_position, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), m_rotation)));
}

void Camera::AddRollPitchYaw(float roll, float pitch, float yaw)
{
    m_roll += roll;
    m_pitch += pitch;
    m_yaw += yaw;

    m_rotation = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
}