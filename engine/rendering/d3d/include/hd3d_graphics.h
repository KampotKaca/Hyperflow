#ifndef HD3D_GRAPHICS_H
#define HD3D_GRAPHICS_H

#include "hd3d_shared.h"

namespace hf
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

	extern GraphicsData GRAPHICS_DATA;

	void Direct3DLoad();
	void Direct3DUnload();
}

#endif //HD3D_GRAPHICS_H
