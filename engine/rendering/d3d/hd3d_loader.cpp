#include <hwindow.h>

#include "hd3d_graphics.h"
#include "exceptions/hgraphicsexception.h"

namespace hf::inter::rendering
{
    GraphicsData GRAPHICS_DATA;

    void Load(const char* version)
    {
        D3D11_HANDLE_EXCEPT(D3D11CreateDevice
        (
            nullptr, // Use default adapter
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr, // No software rasterizer
            0,
            nullptr, // Default feature levels
            0, // Number of feature levels
            D3D11_SDK_VERSION,
            &GRAPHICS_DATA.device,
            nullptr, // Feature level
            &GRAPHICS_DATA.context
        ))

        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.device->QueryInterface(__uuidof(IDXGIDevice), (void**)&GRAPHICS_DATA.dxgiDevice))
        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.dxgiDevice->GetAdapter(&GRAPHICS_DATA.adapter))
        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.adapter->GetParent(__uuidof(IDXGIFactory), (void**)&GRAPHICS_DATA.factory))
    }

    void Unload()
    {
        if(GRAPHICS_DATA.factory) GRAPHICS_DATA.factory->Release();
        if(GRAPHICS_DATA.adapter) GRAPHICS_DATA.adapter->Release();
        if(GRAPHICS_DATA.context) GRAPHICS_DATA.context->Release();
        if(GRAPHICS_DATA.device)  GRAPHICS_DATA.device->Release();
    }

    void* CreateInstance(const Window* window)
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
            .OutputWindow = (HWND)window->handle,
            .Windowed = true,
            .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
            .Flags = 0
        };

        auto instance = new D3DRendererData
        {
            .windowHandle = (HWND)window->handle
        };

        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.factory->CreateSwapChain(GRAPHICS_DATA.device, &dc, &instance->swapChain))

        ID3D11Resource* backBuffer;
        D3D11_HANDLE_EXCEPT(instance->swapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer))
        D3D11_HANDLE_EXCEPT(GRAPHICS_DATA.device->CreateRenderTargetView(backBuffer, nullptr, &instance->renderTexture))

        if(backBuffer) backBuffer->Release();
        return instance;
    }

    void DestroyInstance(void* rnInstance)
    {
        auto instance = (D3DRendererData*)rnInstance;
        if(instance->swapChain)
        {
            instance->swapChain->Release();
            instance->swapChain = nullptr;
        }
    }
}