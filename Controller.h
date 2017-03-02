#pragma once

#include <Windows.h>

class Controller
{
public:
    Controller();
    void OnKeyDown(DWORD vkey);
    void OnKeyUp(DWORD vkey);
    void OnMouseMove(DWORD wParam, int x, int y);

    bool IsKeyDown(char key);
private:
    int m_prevX;
    int m_prevY;

    bool m_keysDown[256];
    bool m_firstMove;
};