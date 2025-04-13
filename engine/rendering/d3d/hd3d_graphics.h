#ifndef HD3D_GRAPHICS_H
#define HD3D_GRAPHICS_H

#include "hd3d_shared.h"
#include "hshared.h"
#include <dxgi.h>

namespace hf::inter::rendering
{
	struct GraphicsData
	{
		int32_t refCount = 0;
		ID3D11Device* device{};
		IDXGIDevice* dxgiDevice{};
		ID3D11DeviceContext* context{};
		IDXGIFactory* factory{};
		IDXGIAdapter* adapter{};
	};

	struct D3DRendererData
	{
		HWND windowHandle{};
		IDXGISwapChain* swapChain{};
		ID3D11RenderTargetView* renderTexture{};
	};

	extern GraphicsData GRAPHICS_DATA;
}

#endif //HD3D_GRAPHICS_H
