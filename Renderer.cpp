#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "Renderer.h"
#include "OculusApp.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace DIRECTX
{
    ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;
}

Renderer::Renderer() :
    m_clientWidth(0),
    m_clientHeight(0),
    m_d3dDevice(nullptr),
    m_d3dDeviceContext(nullptr),
    m_swapChain(nullptr),
    m_backBuffer(nullptr),
    m_backBufferRT(nullptr),
    m_eyeRenderViewport{},
    m_eyeDepthBuffer{},
    m_eyeRenderTexture{},
    m_frameIndex(0),
    m_rendererType(RENDERER),
    m_rasterizerState(RasterizerState::Solid)
{
    m_renderObjects.clear();
}

Renderer::~Renderer()
{
    for (int eye = 0; eye < 2; eye++)
    {
        delete m_eyeRenderTexture[eye];
    }

    if (RendererType::Oculus == m_rendererType)
    {
        ovr_Destroy(m_ovrSession);
        ovr_Shutdown();
    }
}

void Renderer::Initialize(HWND window)
{
    if (RendererType::Oculus == m_rendererType)
    {
        InitializeOVR();
    }

    InitializeD3D(window);
    InitializeShaders();

    InitializeGeometryBuffers();
}

void Renderer::AddRenderObject(Model* model)
{
    MaterialType materialType = model->GetMaterial()->GetType();
    MeshType meshType = model->GetMesh()->GetType();

    if (!ValidateMaterialAndMeshTypes(materialType, meshType))
    {
        throw std::runtime_error("Material and Mesh types are not compatible.");
    }

    // TODO: this is bad jsut initialize them

    // Does the material type exist?
    if (m_renderObjects.find(materialType) == m_renderObjects.end())
    {
        m_renderObjects[materialType] = ModelsByMeshType();
    }

    // Does the mesh type exist?
    if (m_renderObjects[materialType].find(meshType) == m_renderObjects[materialType].end())
    {
        m_renderObjects[materialType][meshType] = Models();
    }

    m_renderObjects[materialType][meshType].push_back(model);
}

void Renderer::Render()
{
    switch (m_rendererType)
    {
    case RendererType::Vitamin:
        RenderVitamin();
        break;
    case RendererType::Oculus:
        RenderOculus();
        break;
    }
}

void Renderer::ToggleRasterizerState()
{
    m_rasterizerState = (RasterizerState::Solid == m_rasterizerState) ? RasterizerState::Wireframe : RasterizerState::Solid;
}

void Renderer::RenderVitamin()
{
    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(10.0f, 10.f, 5.f, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = gOculusApp->GetCamera()->GetViewMatrix();
    XMMATRIX P = XMMatrixPerspectiveFovRH(XM_PI / 4, static_cast<float>(m_clientWidth) / m_clientHeight, .2f, 1000.0f);
    XMMATRIX prod = XMMatrixMultiply(V, P);

    float white[] = { 1.f, 1.f, 1.f, 1.f };
    m_d3dDeviceContext->ClearRenderTargetView(m_backBufferRT.Get(), white);
    m_d3dDeviceContext->ClearDepthStencilView(m_depthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

    RenderScene(&prod);
    
    m_swapChain->Present(0, 0);
}

void Renderer::RenderOculus()
{
    ovrSessionStatus sessionStatus;
    ovr_GetSessionStatus(m_ovrSession, &sessionStatus);
    if (sessionStatus.ShouldQuit)
    {
        throw std::runtime_error("ovrSessionStatus.ShouldQuit");
    }

    if (sessionStatus.ShouldRecenter)
    {
        ovr_RecenterTrackingOrigin(m_ovrSession);
    }

    if (sessionStatus.IsVisible)
    {
        // Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyeOffset) may change at runtime
        ovrEyeRenderDesc eyeRenderDesc[2];
        eyeRenderDesc[0] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Left, m_hmdDesc.DefaultEyeFov[0]);
        eyeRenderDesc[1] = ovr_GetRenderDesc(m_ovrSession, ovrEye_Right, m_hmdDesc.DefaultEyeFov[1]);

        // Get both eye poses simultaneously, with IPD offset already included
        ovrPosef eyeRenderPose[2];
        ovrPosef hmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
                                     eyeRenderDesc[1].HmdToEyePose };;

        // sensorSampleTime is fed into the layer later
        double sensorSampleTime;
        ovr_GetEyePoses(m_ovrSession, m_frameIndex, ovrTrue, hmdToEyePose, eyeRenderPose, &sensorSampleTime);

        // Render Scene to Eye Buffers
        for (int eye = 0; eye < 2; eye++)
        {
            // Clear and set up rendertarget
            float white[] = { 1.f, 1.f, 1.f, 1.f };
            ID3D11RenderTargetView* rtv = m_eyeRenderTexture[eye]->GetRTV();
            m_d3dDeviceContext->OMSetRenderTargets(1, &rtv, m_eyeDepthBuffer[eye].Get());
            m_d3dDeviceContext->ClearRenderTargetView(rtv, white);
            m_d3dDeviceContext->ClearDepthStencilView(m_eyeDepthBuffer[eye].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

            //Get the pose information in XM format
            XMVECTOR eyeQuat = XMVectorSet(
                eyeRenderPose[eye].Orientation.x,
                eyeRenderPose[eye].Orientation.y,
                eyeRenderPose[eye].Orientation.z,
                eyeRenderPose[eye].Orientation.w);
            XMVECTOR eyePos = XMVectorSet(
                eyeRenderPose[eye].Position.x,
                eyeRenderPose[eye].Position.y,
                eyeRenderPose[eye].Position.z,
                0);

            // Get view and projection matrices for the Rift camera
            XMVECTOR CombinedPos = XMVectorAdd(gOculusApp->GetCamera()->GetPosition(), XMVector3Rotate(eyePos, gOculusApp->GetCamera()->GetRotation()));
            Camera finalCam(CombinedPos, XMQuaternionMultiply(gOculusApp->GetCamera()->GetRotation(), eyeQuat));
            XMMATRIX view = finalCam.GetViewMatrix();
            ovrMatrix4f p = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_None);
            XMMATRIX proj = XMMatrixSet(
                p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
                p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
                p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
                p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3]);
            XMMATRIX prod = XMMatrixMultiply(view, proj);

            RenderScene(&prod);

            // Commit rendering to the swap chain
            m_eyeRenderTexture[eye]->Commit();
        }

        // Initialize our single full screen Fov layer
        ovrLayerEyeFov ld = {};
        ld.Header.Type = ovrLayerType_EyeFov;
        ld.Header.Flags = 0;
        for (int eye = 0; eye < 2; ++eye)
        {
            ld.ColorTexture[eye] = m_eyeRenderTexture[eye]->m_textureSwapChain;
            ld.Viewport[eye] = m_eyeRenderViewport[eye];
            ld.Fov[eye] = m_hmdDesc.DefaultEyeFov[eye];
            ld.RenderPose[eye] = eyeRenderPose[eye];
            ld.SensorSampleTime = sensorSampleTime;
        }

        ovrLayerHeader* layers = &ld.Header;
        if (OVR_FAILURE(ovr_SubmitFrame(m_ovrSession, m_frameIndex, nullptr, &layers, 1)))
        {
            throw std::runtime_error("SubmitFrame failed.");
        }

        m_frameIndex++;
    }

    // Render mirror
    ComPtr<ID3D11Texture2D> tex;
    ovr_GetMirrorTextureBufferDX(m_ovrSession, m_mirrorTexture, IID_PPV_ARGS(&tex));
    m_d3dDeviceContext->CopyResource(m_backBuffer.Get(), tex.Get());
    m_swapChain->Present(0, 0);
}

void Renderer::InitializeOVR()
{    
    // Initializes LibOVR, and the Rift
    ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, NULL, 0, 0 };
    if (OVR_FAILURE(ovr_Initialize(&initParams)))
    {
        throw std::runtime_error("ovr_Initialize failed.");
    }

    if (OVR_FAILURE(ovr_Create(&m_ovrSession, &m_gLuid)))
    {
        throw std::runtime_error("ovr_Create failed.");
    }

    m_hmdDesc = ovr_GetHmdDesc(m_ovrSession);
}

void Renderer::InitializeD3D(HWND window)
{
    if (RendererType::Vitamin == m_rendererType)
    {
        m_clientWidth = 1080;
        m_clientHeight = 600;
    }

    if (RendererType::Oculus == m_rendererType)
    {
        m_clientWidth = m_hmdDesc.Resolution.w / 2;
        m_clientHeight = m_hmdDesc.Resolution.h / 2;
    }

    RECT size = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, false);
    const UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
    if (!SetWindowPos(window, nullptr, 0, 0, size.right - size.left, size.bottom - size.top, flags))
    {
        throw std::runtime_error("SetWindowPos failed.");
    }

    IDXGIFactory* pDXGIFactory = nullptr;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)(&pDXGIFactory))))
    {
        throw std::runtime_error("CreateDXGIFactory1 failed.");
    }

    LUID* pLuid = reinterpret_cast<LUID*>(&m_gLuid);

    IDXGIAdapter* pAdapter = nullptr;
    for (UINT iAdapter = 0; pDXGIFactory->EnumAdapters(iAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
    {
        DXGI_ADAPTER_DESC adapterDesc;
        pAdapter->GetDesc(&adapterDesc);
        if ((pLuid == nullptr) || memcmp(&adapterDesc.AdapterLuid, pLuid, sizeof(LUID)) == 0)
        {
            break;
        }
    }

    auto driverType = pAdapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE;
    if (FAILED(D3D11CreateDevice(pAdapter, driverType, 0, 0, 0, 0, D3D11_SDK_VERSION, &m_d3dDevice, 0, &m_d3dDeviceContext)))
    {
        throw std::runtime_error("D3D11CreateDevice failed.");
    }

    DIRECTX::Device = m_d3dDevice.Get();
    DIRECTX::DeviceContext = m_d3dDeviceContext.Get();

    // Create swap chain
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferDesc.Width = m_clientWidth;
    scDesc.BufferDesc.Height = m_clientHeight;
    scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = window;
    scDesc.SampleDesc.Count = 1;
    scDesc.Windowed = true;
    if (RendererType::Vitamin == m_rendererType)
    {
        scDesc.BufferCount = 1;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scDesc.SampleDesc.Quality = 0;
    }
    if (RendererType::Oculus == m_rendererType)
    {
        scDesc.BufferCount = 2;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
    }

    if (FAILED(pDXGIFactory->CreateSwapChain(m_d3dDevice.Get(), &scDesc, &m_swapChain)))
    {
        throw std::runtime_error("CreateSwapChain failed.");
    }

    // Set max frame latency to 1
    ComPtr<IDXGIDevice1> pDXGIDevice1;
    if (FAILED(m_d3dDevice.As(&pDXGIDevice1)))
    {
        throw std::runtime_error("m_d3dDevice.As(&pDXGIDevice1) failed.");
    }
    pDXGIDevice1->SetMaximumFrameLatency(1);

    // Create backbuffer
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_backBuffer);
    if (FAILED(m_d3dDevice->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_backBufferRT)))
    {
        throw std::runtime_error("CreateRenderTargetView failed.");
    }

    // Main depth buffer
    CreateDepthBuffer(m_clientWidth, m_clientHeight, 1, &m_depthBuffer);

    m_d3dDeviceContext->OMSetRenderTargets(1, m_backBufferRT.GetAddressOf(), m_depthBuffer.Get());

    if (RendererType::Oculus == m_rendererType)
    {
        for (int eye = 0; eye < 2; eye++)
        {
            ovrSizei idealSize = ovr_GetFovTextureSize(m_ovrSession, (ovrEyeType)eye, m_hmdDesc.DefaultEyeFov[eye], 1.0f);
            m_eyeRenderTexture[eye] = new OculusTexture();
            m_eyeRenderTexture[eye]->Init(m_ovrSession, idealSize.w, idealSize.h);
            CreateDepthBuffer(idealSize.w, idealSize.h, 1, &m_eyeDepthBuffer[eye]);
            m_eyeRenderViewport[eye].Pos.x = 0;
            m_eyeRenderViewport[eye].Pos.y = 0;
            m_eyeRenderViewport[eye].Size = idealSize;
            if (!m_eyeRenderTexture[eye]->m_textureSwapChain)
            {
                throw std::runtime_error("Create texture failed.");
            }
        }

        // Create a mirror to see on the monitor.
        ovrMirrorTextureDesc mirrorDesc = {};
        mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        mirrorDesc.Width = m_clientWidth;
        mirrorDesc.Height = m_clientHeight;
        if (OVR_FAILURE(ovr_CreateMirrorTextureDX(m_ovrSession, m_d3dDevice.Get(), &mirrorDesc, &m_mirrorTexture)))
        {
            throw std::runtime_error("Create mirror texture failed.");
        }

        // FloorLevel will give tracking poses where the floor height is 0
        ovr_SetTrackingOriginType(m_ovrSession, ovrTrackingOrigin_FloorLevel);
    }

    // Set viewport
    if (RendererType::Vitamin == m_rendererType)
    {
        D3D11_VIEWPORT mScreenViewport = {};
        mScreenViewport.TopLeftX = 0;
        mScreenViewport.TopLeftY = 0;
        mScreenViewport.Width = static_cast<float>(m_clientWidth);
        mScreenViewport.Height = static_cast<float>(m_clientHeight);
        mScreenViewport.MinDepth = 0.0f;
        mScreenViewport.MaxDepth = 1.0f;
        m_d3dDeviceContext->RSSetViewports(1, &mScreenViewport);
    }
    if (RendererType::Oculus == m_rendererType)
    {
        for (int eye = 0; eye < 2; eye++)
        {
            // Set viewport
            D3D11_VIEWPORT vp;
            vp.Width = static_cast<float>(m_eyeRenderViewport[eye].Size.w);
            vp.Height = static_cast<float>(m_eyeRenderViewport[eye].Size.h);
            vp.MinDepth = 0;
            vp.MaxDepth = 1;
            vp.TopLeftX = static_cast<float>(m_eyeRenderViewport[eye].Pos.x);
            vp.TopLeftY = static_cast<float>(m_eyeRenderViewport[eye].Pos.y);
            m_d3dDeviceContext->RSSetViewports(1, &vp);
        }
    }
}

void Renderer::InitializeShaders()
{
    // Initialize color shader
    Shaders::ColorShader = new ColorShader();

    // Create rasterizers
    D3D11_RASTERIZER_DESC rs = {};
    rs.AntialiasedLineEnable = true;
    rs.DepthClipEnable = true;
    rs.CullMode = D3D11_CULL_NONE;
    rs.FrontCounterClockwise = true;
    rs.FillMode = D3D11_FILL_WIREFRAME;
    rs.MultisampleEnable = true;
    THROW_IF_FAILED(m_d3dDevice->CreateRasterizerState(&rs, &m_wireframeRasterizer), "CreateRasterizerState failed.");

    rs = {};
    rs.AntialiasedLineEnable = true;
    rs.DepthClipEnable = true;
    rs.CullMode = D3D11_CULL_BACK;
    rs.FrontCounterClockwise = true;
    rs.FillMode = D3D11_FILL_SOLID;
    rs.MultisampleEnable = true;
    THROW_IF_FAILED(m_d3dDevice->CreateRasterizerState(&rs, &m_solidRasterizer), "CreateRasterizerState failed.");
    
    // Create depth state
    D3D11_DEPTH_STENCIL_DESC ds = {};
    ds.DepthEnable = true;
    ds.DepthFunc = D3D11_COMPARISON_LESS;
    ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    THROW_IF_FAILED(m_d3dDevice->CreateDepthStencilState(&ds, &m_depthState), "CreateDepthStencilState failed.");
}

void Renderer::InitializeGeometryBuffers()
{
    MeshData<ColorVertex> boxColorMesh;
    GeometryBuilder::CreateBoxMesh(1.f, 1.f, 1.f, Colors::White, boxColorMesh);
    Meshes::Primitives::BoxColorMesh = new Mesh(m_d3dDevice.Get(), boxColorMesh);

    MeshData<NormalVertex> boxNormalVertex;
    GeometryBuilder::CreateBoxMesh(1.f, 1.f, 1.f, boxNormalVertex);
    Meshes::Primitives::BoxNormalMesh = new Mesh(m_d3dDevice.Get(), boxNormalVertex);

    MeshData<ColorVertex> axisMesh;
    GeometryBuilder::CreateAxisMesh(1.f, 1.f, 1.f, axisMesh);
    Meshes::Primitives::AxisMesh = new Mesh(m_d3dDevice.Get(), axisMesh);
}

void Renderer::RenderScene(XMMATRIX* projView)
{
    if (m_renderObjects.size() == 0)
    {
        return;
    }

    SetRasterizerState(m_rasterizerState);

    m_d3dDeviceContext->OMSetDepthStencilState(m_depthState.Get(), 0);

    // Iterate through the render objects.
    for (auto itByMaterialType = m_renderObjects.begin(); itByMaterialType != m_renderObjects.end(); itByMaterialType++)
    {
        for (auto itByMeshType = itByMaterialType->second.begin(); itByMeshType != itByMaterialType->second.end(); itByMeshType++)
        {
            for (auto itModels = itByMeshType->second.begin(); itModels != itByMeshType->second.end(); itModels++)
            {
                Model* model = *itModels;

                SetShader(model->GetMaterial()->GetShader());

                SetVertexIndexBuffers(
                    model->GetMesh()->GetVertexBuffer(),
                    model->GetMesh()->GetIndexBuffer(),
                    model->GetMesh()->GetVertexSize(),
                    model->GetMesh()->GetTopology());
                
                if (model->GetMaterial()->GetType() == SolidColor)
                {

                    XMMATRIX modelMat = XMMatrixMultiply(
                        XMMatrixRotationQuaternion(XMLoadFloat4((&model->GetRotation()))),
                        XMMatrixTranslationFromVector(XMLoadFloat3(&(model->GetPosition()))));

                    ColorMaterial* material = (ColorMaterial*)(model->GetMaterial());
                    material->GetShader()->UpdateConstantBuffer(XMMatrixMultiply(modelMat, *projView), material->GetColor());
                    
                    DIRECTX::DeviceContext->DrawIndexed(model->GetMesh()->GetIndexCount(), 0, 0);
                }
            }
        }
    }
}

inline void Renderer::SetShader(Shader* shader)
{
    static Shader* currentShader = nullptr;

    if (shader != currentShader)
    {
        shader->Set();
        
        currentShader = shader;
    }
}

inline void Renderer::SetRasterizerState(RasterizerState rasterizerState)
{
    static RasterizerState currentRasterizerState = RasterizerState::None;

    if (rasterizerState != currentRasterizerState)
    {
        if (RasterizerState::Solid == rasterizerState)
        {
            m_d3dDeviceContext->RSSetState(m_solidRasterizer.Get());
        }
        else if (RasterizerState::Wireframe == rasterizerState)
        {
            m_d3dDeviceContext->RSSetState(m_wireframeRasterizer.Get());
        }

        currentRasterizerState = rasterizerState;
    }
}

void Renderer::SetVertexIndexBuffers(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, size_t * vertexSize, D3D_PRIMITIVE_TOPOLOGY topology)
{
    static ID3D11Buffer * s_vertexBuffer = nullptr;
    static ID3D11Buffer * s_indexBuffer = nullptr;
    static D3D_PRIMITIVE_TOPOLOGY s_topology;

    if (vertexBuffer == s_vertexBuffer && indexBuffer == s_indexBuffer)
    {
        return;
    }

    if (vertexBuffer != s_vertexBuffer)
    {
        UINT offset = 0;
        UINT size = static_cast<UINT>(*vertexSize);
        m_d3dDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &size, &offset);
    }

    if (indexBuffer != s_indexBuffer)
    {
        m_d3dDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    }

    m_d3dDeviceContext->IASetPrimitiveTopology(topology);

    s_vertexBuffer = vertexBuffer;
    s_indexBuffer = indexBuffer;
}

bool Renderer::ValidateMaterialAndMeshTypes(MaterialType materialType, MeshType meshType)
{
    // There is an implicit relationship between the material and mesh types. An incompatibility
    // may not necessarily manifest itself as crash so do some verification here if DEBUG is enabled.

    return true;
}

inline void Renderer::CreateDepthBuffer(int width, int height, int sampleCount, ID3D11DepthStencilView **ppDepthStencilView)
{
    D3D11_TEXTURE2D_DESC dsDesc = {};
    dsDesc.Width = width;
    dsDesc.Height = height;
    dsDesc.MipLevels = 1;
    dsDesc.ArraySize = 1;
    dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsDesc.SampleDesc.Count = sampleCount;
    dsDesc.SampleDesc.Quality = 0;
    dsDesc.Usage = D3D11_USAGE_DEFAULT;
    dsDesc.CPUAccessFlags = 0;
    dsDesc.MiscFlags = 0;
    dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ComPtr<ID3D11Texture2D> tex;
    m_d3dDevice->CreateTexture2D(&dsDesc, NULL, &tex);
    m_d3dDevice->CreateDepthStencilView(tex.Get(), NULL, ppDepthStencilView);
}
