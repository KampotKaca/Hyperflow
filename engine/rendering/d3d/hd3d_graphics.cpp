#include "hd3d_graphics.h"

namespace hf
{
	int32_t Graphics::s_RefCount = 0;
	IDXGIDevice* Graphics::s_Device = nullptr;
	ID3D11DeviceContext* Graphics::s_Context = nullptr;

	Graphics::Graphics(HWND windowHandle) : m_WindowHandle(windowHandle)
	{
		if(s_RefCount == 0) Init();
		s_RefCount++;
	}

	Graphics::~Graphics()
	{
		s_RefCount--;
		if(s_RefCount == 0) Dispose();
	}

	void Graphics::Init()
	{

	}

	void Graphics::Dispose()
	{

	}
}