#include <stdio.h>

#include "OculusApp.h"
#include "Controller.h"

// TODO: should not be here
#include <DirectXMath.h>
using namespace DirectX;

Controller::Controller() :
    m_firstMove(true)
{
}

void Controller::OnKeyDown(DWORD vkey)
{
    Camera* camera = gOculusApp->GetCamera();

    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -0.05f, 0), camera->GetRotation());
    XMVECTOR right = XMVector3Rotate(XMVectorSet(0.05f, 0, 0, 0), camera->GetRotation());

    switch (vkey)
    {
    case 'W':
        camera->SetPosition(XMVectorAdd(camera->GetPosition(), forward));
        break;
    case 'A':
        camera->SetPosition(XMVectorSubtract(camera->GetPosition(), right));
        break;
    case 'S':
        camera->SetPosition(XMVectorSubtract(camera->GetPosition(), forward));
        break;
    case 'D':
        camera->SetPosition(XMVectorAdd(camera->GetPosition(), right));
        break;
    default:
        break;
    }

    m_keysDown[static_cast<int>(vkey)] = true;
}

void Controller::OnKeyUp(DWORD vkey)
{
    m_keysDown[static_cast<int>(vkey)] = false;
}

void Controller::OnMouseMove(DWORD wParam, int x, int y)
{
    if ((wParam & MK_LBUTTON) != 0)
    {
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_prevX));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_prevY));

        Camera* camera = gOculusApp->GetCamera();

        camera->AddRollPitchYaw(0.f, -dy, -dx);
    }

    if (m_firstMove)
    {
        m_firstMove = false;
    }
    else
    {

    }

    m_prevX = x;
    m_prevY = y;
}

bool Controller::IsKeyDown(char key)
{
    return m_keysDown[static_cast<int>(key)];
}
