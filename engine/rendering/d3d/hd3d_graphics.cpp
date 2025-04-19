#include "hd3d_graphics.h"

#include "hrenderer.h"
#include "exceptions/hgraphicsexception.h"
#include "hyperflow.h"

namespace hf::inter::rendering
{
	bool StartFrame(Renderer* rn)
	{
		auto rnData = (D3DRendererData*)rn->handle;
		float c[4] = { glm::abs(std::sin((float)time::GetTimePassed())), 0, 0, 1 };
		GRAPHICS_DATA.context->ClearRenderTargetView(rnData->renderTexture, c);
		return true;
	}

	void EndFrame(Renderer* rn)
	{
		auto rnData = (D3DRendererData*)rn->handle;
		D3D11_HANDLE_EXCEPT(rnData->swapChain->Present(1u, 0));
	}

	void Draw(Renderer* rn)
	{

	}
}