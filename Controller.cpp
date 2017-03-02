#include <stdio.h>
#include <algorithm>

#include "OculusApp.h"
#include "Controller.h"

Controller::Controller() :
    m_firstMove(true)
{
    std::fill(m_keysDown, m_keysDown + ARRAYSIZE(m_keysDown), false);
}

void Controller::OnKeyDown(DWORD vkey)
{
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
        Camera* camera = gOculusApp->GetCamera();
        camera->MouseLook(x - m_prevX, y - m_prevY);
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
