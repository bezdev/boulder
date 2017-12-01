#include "OculusApp.h"

#define APP_NAME "boulder"

extern OculusApp* gOculusApp = nullptr;

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return gOculusApp->WindowProc(hwnd, msg, wParam, lParam);
}

OculusApp::OculusApp(HINSTANCE hInstance) :
    m_hInstance(hInstance),
    m_window(nullptr),
    m_isRunning(false)
{
    gOculusApp = this;

    m_controller = new Controller();
    m_renderer = new Renderer();
}

OculusApp::~OculusApp()
{
    delete m_controller;
    m_controller = nullptr;

    delete m_renderer;
    m_renderer = nullptr;

    delete m_scene;
    m_scene = nullptr;

    delete m_camera;
    m_camera = nullptr;

    delete m_globalTimer;
    m_globalTimer = nullptr;
}

void OculusApp::Initialize()
{
    InitializeWindow();
    m_renderer->Initialize(m_window);

    m_scene = SceneManager::CreateColorCubeScene();

    m_camera = new Camera(DirectX::XMVectorSet(0.0f, 0.0f, 10.0f, 0), DirectX::XMQuaternionIdentity());
}

void OculusApp::Run()
{
    m_globalTimer = new Timer();
    static bool firstMainLoopIt = true;

    // main loop
    while (HandleMessages())
    {
        if (firstMainLoopIt)
        {
            m_globalTimer->Reset();
        }
        else
        {
            m_globalTimer->Update();
        }
        
        UpdateControllerState();

        m_renderer->Render();

        CalculateFrameStats();

        firstMainLoopIt = false;
    }
}

void OculusApp::InitializeWindow()
{
    WNDCLASS wc = {};
    wc.style         = CS_OWNDC;
    wc.lpszClassName = APP_NAME;
    wc.lpfnWndProc   = MainWindowProc;
    wc.cbWndExtra = 0;

    if (!RegisterClass(&wc))
    {
        throw std::runtime_error("RegisterClassW failed.");
    }

    m_window = CreateWindow(
        wc.lpszClassName,
        wc.lpszClassName,
        WS_OVERLAPPEDWINDOW,
        0, 0,
        0, 0,
        nullptr,
        nullptr,
        m_hInstance,
        nullptr);
    if (!m_window)
    {
        throw std::runtime_error("CreateWindowW failed.");
    }

    m_isRunning = true;
}

void OculusApp::UpdateControllerState()
{
    if (m_controller->IsKeyDown('W'))
    {
        m_camera->Move(MoveDirection::Forward);
    }
    if (m_controller->IsKeyDown('A'))
    {
        m_camera->Move(MoveDirection::Left);
    }
    if (m_controller->IsKeyDown('S'))
    {
        m_camera->Move(MoveDirection::Back);
    }
    if (m_controller->IsKeyDown('D'))
    {
        m_camera->Move(MoveDirection::Right);
    }
}

void OculusApp::CalculateFrameStats()
{
    static int frameCount = 0;
    static float timeElapsed = 0.0f;

    frameCount++;
    timeElapsed += m_globalTimer->GetDelta();

    if (timeElapsed >= 1000.0f)
    {
        float fps = frameCount / timeElapsed * 1000.f;
        float mspf = timeElapsed / fps;

        wchar_t titleBuffer[99];
        swprintf_s(titleBuffer, L"FPS: %.0f", round(fps));
        SetWindowTextW(m_window, titleBuffer);

        frameCount = 0;
        timeElapsed -= 1000.0f;
    }
}

bool OculusApp::HandleMessages()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

LRESULT OculusApp::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_KEYDOWN:
        m_controller->OnKeyDown(wParam);
        return 0;
    case WM_KEYUP:
        m_controller->OnKeyUp(wParam);
        return 0;
    case WM_MOUSEMOVE:
        m_controller->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_ACTIVATE:
        if ((LOWORD(wParam) == WA_INACTIVE)) { }
        return 0;
    case WM_SIZE:
        return 0;
    case WM_DESTROY:
        m_isRunning = false;

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}