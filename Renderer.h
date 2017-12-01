#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <map>
#include <vector>

#include "OVR_CAPI_D3D.h"

#include "Shader.h"
#include "Model.h"

#define RENDERER RendererType::Vitamin

namespace DIRECTX
{
    extern ID3D11Device* Device;
    extern ID3D11DeviceContext* DeviceContext;
}

typedef std::vector<Model*> Models;
typedef std::map<MeshType, Models> ModelsByMeshType;
typedef std::map<MaterialType, ModelsByMeshType> ModelsByMaterialByMeshType;
typedef ModelsByMaterialByMeshType RenderObjects;

enum RendererType
{
    Vitamin,
    Oculus
};

enum RasterizerState
{
    None,
    Solid,
    Wireframe
};

struct OculusTexture
{
    ovrSession m_ovrSession;
    ovrTextureSwapChain m_textureSwapChain;
    std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_texRTV;

    OculusTexture() :
        m_ovrSession(nullptr),
        m_textureSwapChain(nullptr)
    {
    }

    void Init(ovrSession session, ID3D11Device* device, int sizeW, int sizeH)
    {
        m_ovrSession = session;

        ovrTextureSwapChainDesc desc = {};
        desc.Type = ovrTexture_2D;
        desc.ArraySize = 1;
        desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
        desc.Width = sizeW;
        desc.Height = sizeH;
        desc.MipLevels = 1;
        desc.SampleCount = 1;
        desc.MiscFlags = ovrTextureMisc_DX_Typeless;
        desc.BindFlags = ovrTextureBind_DX_RenderTarget;
        desc.StaticImage = ovrFalse;

        if (OVR_FAILURE(ovr_CreateTextureSwapChainDX(m_ovrSession, device, &desc, &m_textureSwapChain)))
        {
            throw std::runtime_error("ovr_CreateTextureSwapChainDX failed.");
        }

        int textureCount = 0;
        ovr_GetTextureSwapChainLength(m_ovrSession, m_textureSwapChain, &textureCount);
        for (int i = 0; i < textureCount; i++)
        {
            Microsoft::WRL::ComPtr<ID3D11Texture2D> tex = nullptr;
            ovr_GetTextureSwapChainBufferDX(m_ovrSession, m_textureSwapChain, i, IID_PPV_ARGS(&tex));
            D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
            rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            ID3D11RenderTargetView* rtv;
            device->CreateRenderTargetView(tex.Get(), &rtvd, &rtv);
            m_texRTV.push_back(rtv);
        }
    }

    ~OculusTexture()
    {
        if (m_textureSwapChain)
        {
            ovr_DestroyTextureSwapChain(m_ovrSession, m_textureSwapChain);
        }
    }

    ID3D11RenderTargetView* GetRTV()
    {
        int index = 0;
        ovr_GetTextureSwapChainCurrentIndex(m_ovrSession, m_textureSwapChain, &index);
        return m_texRTV[index].Get();
    }

    void Commit()
    {
        ovr_CommitTextureSwapChain(m_ovrSession, m_textureSwapChain);
    }
};

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void Initialize(HWND window);
    void AddRenderObject(Model* model);
    void Render();

    void ToggleRasterizerState();
protected:
    void InitializeOVR();
    void InitializeD3D(HWND window);
    void InitializeShaders();
    void InitializeGeometryBuffers();

    void CreateDepthBuffer(int width, int height, int sampleCount, ID3D11DepthStencilView **ppDepthStencilView);

    void RenderVitamin();
    void RenderOculus();
    void RenderScene(DirectX::XMMATRIX* projView);

    void SetShader(Shader* shader);
    void SetRasterizerState(RasterizerState rasterizerState);
    void SetVertexIndexBuffers(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, size_t* vertexSize, D3D_PRIMITIVE_TOPOLOGY topology);

    bool ValidateMaterialAndMeshTypes(MaterialType materialType, MeshType meshType);

    int m_clientWidth;
    int m_clientHeight;

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRT;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthState;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframeRasterizer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_solidRasterizer;
    RasterizerState m_rasterizerState;

    // Render objects are first sorted by material type and then mesh type to
    // minimize changing the rendering states.
    RenderObjects m_renderObjects;

    ovrSession m_ovrSession;
    ovrHmdDesc m_hmdDesc;
    ovrGraphicsLuid m_gLuid;
    ovrMirrorTexture m_mirrorTexture;
    ovrRecti m_eyeRenderViewport[2];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_eyeDepthBuffer[2];
    OculusTexture* m_eyeRenderTexture[2];
    long long m_frameIndex;

    RendererType m_rendererType;
};