#pragma once

#include <wrl.h>
#include <D3D11.h>
#include <vector>

#include <DirectXMath.h>
#include "OVR_CAPI_D3D.h"

enum RendererType
{
    Vitamin,
    Oculus
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

    // Commit changes
    void Commit()
    {
        ovr_CommitTextureSwapChain(m_ovrSession, m_textureSwapChain);
    }
};

class Renderer
{
public:
    Renderer(RendererType rendererType);
    ~Renderer();
    void Initialize(HWND window);
    void Render();

    ID3D11Device* GetDevice()
    {
        return m_d3dDevice.Get();
    }

    ID3D11DeviceContext* GetDeviceContext()
    {
        return m_d3dDeviceContext.Get();
    }

    ID3D11Buffer* GetConstantBuffer()
    {
        return m_constantBuffer.Get();
    }

protected:
    void InitializeOVR();
    void InitializeD3D(HWND window);
    void InitializeShaders();

    void RenderVitamin();
    void RenderOculus();
    void RenderScene(DirectX::XMMATRIX* projView);

    void CreateDepthBuffer(int width, int height, int sampleCount, ID3D11DepthStencilView **ppDepthStencilView);

    int m_clientWidth;
    int m_clientHeight;

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRT;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthState;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

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