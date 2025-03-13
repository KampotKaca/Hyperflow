#ifndef HD3D_GRAPHICS_H
#define HD3D_GRAPHICS_H

#include "hd3d_shared.h"
#include "hshared.h"
#include <dxgi.h>

namespace hf
{
	class Graphics
	{
	public:
		Graphics(HWND windowHandle);
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();

		void StartFrame();
		void EndFrame();

		void ClearFrame(const glm::vec4& color);

		static ID3D11DeviceContext* GetContext();

	private:
		HWND m_WindowHandle;
		IDXGISwapChain* m_SwapChain;
		ID3D11RenderTargetView* m_RenderTexture;

		static int32_t s_RefCount;
		static ID3D11Device* s_Device;
		static IDXGIDevice* s_DxgiDevice;
		static ID3D11DeviceContext* s_Context;
		static IDXGIFactory* s_Factory;
		static IDXGIAdapter* s_Adapter;

		static void Init();
		static void Dispose();
	};
}

#endif //HD3D_GRAPHICS_H
