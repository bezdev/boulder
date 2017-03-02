#include "Camera.h"
#include "OculusApp.h"

using namespace DirectX;

Camera::Camera(XMVECTOR* position, XMVECTOR* rotation):
    m_position(*position),
    m_rotation(*rotation),
    m_roll(0.f),
    m_pitch(0.f),
    m_yaw(0.f),
    m_velocity(5.0f / 1000)
{
}

Camera::Camera(const XMVECTOR& position, const XMVECTOR& rotation):
    m_position(position),
    m_rotation(rotation),
    m_roll(0.f),
    m_pitch(0.f),
    m_yaw(0.f),
    m_velocity(5.0f / 1000)
{
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -1, 0), m_rotation);
    return (XMMatrixLookAtRH(m_position, XMVectorAdd(m_position, forward), XMVector3Rotate(XMVectorSet(0, 1, 0, 0), m_rotation)));
}

void Camera::MouseLook(int dx, int dy)
{
    float yaw = XMConvertToRadians(0.25f * static_cast<float>(dx));
    float pitch = XMConvertToRadians(0.25f * static_cast<float>(dy));

    AddRollPitchYaw(0.f, -pitch, -yaw);
}

void Camera::AddRollPitchYaw(float roll, float pitch, float yaw)
{
    m_roll += roll;
    m_pitch += pitch;
    m_yaw += yaw;

    m_rotation = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, m_roll);
}

void Camera::Move(MoveDirection moveDirection)
{
    float delta = gOculusApp->GetGlobalTimer()->GetDelta();

    float d = (delta * m_velocity);
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -d, 0), m_rotation);
    XMVECTOR right = XMVector3Rotate(XMVectorSet(d, 0, 0, 0), m_rotation);

    switch (moveDirection)
    {
    case MoveDirection::Forward:
        SetPosition(XMVectorAdd(m_position, forward));
        break;
    case MoveDirection::Left:
        SetPosition(XMVectorSubtract(m_position, right));
        break;
    case MoveDirection::Back:
        SetPosition(XMVectorSubtract(m_position, forward));
        break;
    case MoveDirection::Right:
        SetPosition(XMVectorAdd(m_position, right));
        break;
    }
}
