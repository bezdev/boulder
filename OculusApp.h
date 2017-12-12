#pragma once

#include <Windows.h>
#include <vector>
#include <D3D11.h>
#include <stdexcept>

#include "OVR_CAPI_D3D.h"

#include "bez.h"
#include "Controller.h"
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "Logger.h"

class OculusApp
{
public:
    OculusApp(HINSTANCE hInstance, bool testMode = false);
    ~OculusApp();

    void Initialize();
    void Run();

    bool HandleMessages();
    LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND GetHWND() { return m_window; }

    Renderer* GetRenderer() { return m_renderer; };
    Scene* GetScene() { return m_scene; };
    Camera* GetCamera() { return m_camera; };
    Timer* GetGlobalTimer() { return m_globalTimer; };
    Logger* GetLogger() { return m_logger; }

    bool IsTest() { return m_isTestMode; };
protected:
    void InitializeWindow();

    void UpdateControllerState();
    void CalculateFrameStats();

    HINSTANCE m_hInstance;
    HWND m_window;
    bool m_isRunning;
    bool m_isTestMode;

    Controller* m_controller;
    Renderer* m_renderer;
    Scene* m_scene;
    Camera* m_camera;
    Timer* m_globalTimer;
    Logger* m_logger;
};

extern OculusApp* gOculusApp;