#ifndef HD3D_GRAPHICS_H
#define HD3D_GRAPHICS_H

#include "hd3d_shared.h"
#include "hshared.h"

namespace hf
{
	class Graphics
	{
	public:
		Graphics(HWND windowHandle);
		~Graphics();

	private:
		HWND m_WindowHandle;
		IDXGISwapChain* m_SwapChain;

		static int32_t s_RefCount;
		static IDXGIDevice* s_Device;
		static ID3D11DeviceContext* s_Context;

		static void Init();
		static void Dispose();
	};
}

#endif //HD3D_GRAPHICS_H
