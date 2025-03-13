#include "hd3d_graphics.h"
#include "exceptions/hgraphicsexception.h"
#include "components/htime.h"

namespace hf
{
	int32_t Graphics::s_RefCount = 0;
	ID3D11Device* Graphics::s_Device = nullptr;
	IDXGIDevice* Graphics::s_DxgiDevice = nullptr;
	ID3D11DeviceContext* Graphics::s_Context = nullptr;
	IDXGIFactory* Graphics::s_Factory = nullptr;
	IDXGIAdapter* Graphics::s_Adapter = nullptr;

	Graphics::Graphics(HWND windowHandle) : m_WindowHandle(windowHandle)
	{
		if(s_RefCount == 0) Init();
		s_RefCount++;

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
			.OutputWindow = m_WindowHandle,
			.Windowed = true,
			.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
			.Flags = 0
		};

		D3D11_HANDLE_EXCEPT(s_Factory->CreateSwapChain(s_Device, &dc, &m_SwapChain))

		ID3D11Resource* backBuffer;
		D3D11_HANDLE_EXCEPT(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&backBuffer))
		D3D11_HANDLE_EXCEPT(s_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTexture))

		if(backBuffer) backBuffer->Release();
	}

	Graphics::~Graphics()
	{
		if(m_SwapChain) m_SwapChain->Release();

		s_RefCount--;
		if(s_RefCount == 0) Dispose();
	}

	void Graphics::Init()
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
			&s_Device,
			nullptr, // Feature level
			&s_Context
		))

		D3D11_HANDLE_EXCEPT(s_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&s_DxgiDevice))
		D3D11_HANDLE_EXCEPT(s_DxgiDevice->GetAdapter(&s_Adapter))
		D3D11_HANDLE_EXCEPT(s_Adapter->GetParent(__uuidof(IDXGIFactory), (void**)&s_Factory))
	}

	void Graphics::Dispose()
	{
		if(s_Factory) s_Factory->Release();
		if(s_Adapter) s_Adapter->Release();
		if(s_Context) s_Context->Release();
		if(s_Device) s_Device->Release();
	}

	void Graphics::StartFrame()
	{
		ClearFrame({ glm::abs(std::sin((float)Time::GetTimePassedAfterLaunch())), 0, 0, 1 });
	}

	void Graphics::EndFrame()
	{
		m_SwapChain->Present(1u, 0);
	}

	void Graphics::ClearFrame(const glm::vec4 &color)
	{
		float c[4] = { color.r, color.g, color.b, color.a };
		s_Context->ClearRenderTargetView(m_RenderTexture, c);
	}

	ID3D11DeviceContext *Graphics::GetContext() { return s_Context; }
}