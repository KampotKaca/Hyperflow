#ifndef HD3D_RENDERER_H
#define HD3D_RENDERER_H

#include "hd3d_graphics.h"

namespace hf
{
    struct D3DRenderer
    {
        HWND windowHandle{};
        IDXGISwapChain* swapChain{};
        ID3D11RenderTargetView* renderTexture{};
    };

    D3DRenderer* CreateRenderer(HWND window);
    void DestroyRenderer(D3DRenderer* instance);

    bool StartFrame(D3DRenderer* rn);
    void EndFrame(D3DRenderer* rn);
    void RegisterFrameBufferChange(D3DRenderer* rn, uvec2 newSize);
    void Draw(D3DRenderer* rn);
}

#endif //HD3D_RENDERER_H
