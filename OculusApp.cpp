#include "OculusApp.h"
#include <windowsx.h>
#define APP_NAME L"boulder"

// TODO: should not be using direct x in this file
using namespace DirectX;

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
    m_renderer = new Renderer(RendererType::Vitamin);
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
}

void OculusApp::Initialize()
{
    InitializeWindow();
    m_renderer->Initialize(m_window);

    m_scene = new Scene();

    MeshData boxMesh;
    CreateBoxMesh(5.f, 5.f, 5.f, boxMesh);
    m_scene->AddModel(new Model(m_renderer->GetDevice(), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), boxMesh));

    MeshData unitAxis;
    CreateAxisMesh(5.f, 5.f, 5.f, unitAxis);
    m_scene->AddModel(new Model(m_renderer->GetDevice(), XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT4(0.f, 0.f, 0.f, 0.f), unitAxis));

    m_camera = new Camera(XMVectorSet(0.0f, 0.0f, 10.0f, 0), XMQuaternionIdentity());
}

void OculusApp::Run()
{
    while (HandleMessages())
    {
        // controller
        // update
        m_renderer->Render();
    }
}

void OculusApp::InitializeWindow()
{
    WNDCLASSW wc = {};
    wc.style         = CS_OWNDC;
    wc.lpszClassName = APP_NAME;
    wc.lpfnWndProc   = MainWindowProc;
    wc.cbWndExtra = 0;

    if (!RegisterClassW(&wc))
    {
        throw std::runtime_error("RegisterClassW failed.");
    }

    m_window = CreateWindowW(
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