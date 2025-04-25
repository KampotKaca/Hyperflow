#include "../include/hd3d_graphics.h"
#include "../include/hd3d_renderer.h"

namespace hf
{
    GraphicsData GRAPHICS_DATA;

    void Direct3DLoad()
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

    void Direct3DUnload()
    {
        if(GRAPHICS_DATA.factory) GRAPHICS_DATA.factory->Release();
        if(GRAPHICS_DATA.adapter) GRAPHICS_DATA.adapter->Release();
        if(GRAPHICS_DATA.context) GRAPHICS_DATA.context->Release();
        if(GRAPHICS_DATA.device)  GRAPHICS_DATA.device->Release();
    }
}