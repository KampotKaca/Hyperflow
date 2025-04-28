#include "../include/hd3d_renderer.h"

namespace hf
{
    D3DRenderer* CreateRenderer(HWND window)
    {
        DXGI_SWAP_CHAIN_DESC dc =
        {
            .BufferDesc =
            {
                .Width = 0,
                .Height = 0,
                .RefreshRate =
                    {
                    .Numerator = 0,
                    .Denominator = 0,
                },
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            .Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
        },
        .SampleDesc =
        {
                .Count = 1,
                .Quality = 0
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 1,
            .OutputWindow = window,
            .Windowed = true,
            .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
            .Flags = 0
        };

        auto instance = new D3DRenderer
        {
            .windowHandle = window
        };

        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.factory->CreateSwapChain(GRAPHICS_DATA.device, &dc, &instance->swapChain))

        ID3D11Resource* backBuffer;
        D3D11_HANDLE_EXCEPT(instance->swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer))
        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.device->CreateRenderTargetView(backBuffer, nullptr, &instance->renderTexture))

        if(backBuffer) backBuffer->Release();
        return instance;
    }

    void DestroyRenderer(D3DRenderer* instance)
    {
        if(instance->swapChain)
        {
            instance->swapChain->Release();
            instance->swapChain = nullptr;
        }
    }

    bool GetReadyForRendering(D3DRenderer* rn)
    {
        float c[4] = { 1, 0, 0, 1 };
        GRAPHICS_DATA.context->ClearRenderTargetView(rn->renderTexture, c);
        return true;
    }

    void StartFrame(D3DRenderer* rn)
    {

    }

    void EndFrame(D3DRenderer* rn)
    {
        D3D11_HANDLE_EXCEPT(rn->swapChain->Present(1u, 0));
    }

    void RegisterFrameBufferChange(D3DRenderer* rn, uvec2 newSize)
    {

    }

    void Draw(D3DRenderer* rn)
    {

    }
}