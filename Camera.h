#pragma once

#include <cstddef>
#include <DirectXMath.h>

__declspec(align(16)) class Camera
{
public:
    Camera(DirectX::XMVECTOR* position, DirectX::XMVECTOR* rotation);
    Camera(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& rotation);

    void* operator new(std::size_t)
    {
        return _aligned_malloc(sizeof(Camera), __alignof(Camera));
    }

    void operator delete(void* p)
    {
        _aligned_free(p);
    }

    DirectX::XMMATRIX GetViewMatrix();

    DirectX::XMVECTOR GetPosition() { return m_position; };
    void SetPosition(DirectX::XMVECTOR value) { m_position = value; };

    DirectX::XMVECTOR GetRotation() { return m_rotation; };

    void AddRollPitchYaw(float roll, float pitch, float yaw);
protected:
    DirectX::XMVECTOR m_position;
    DirectX::XMVECTOR m_rotation;

    float m_roll;
    float m_pitch;
    float m_yaw;
};