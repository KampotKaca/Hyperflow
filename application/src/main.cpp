#include "hyperflow.h"
#include "application.h"
#include "renderpasses.h"

static void __attribute__((constructor)) global_init()
{
	hf::Preload();
}

int main()
{
	hf::EngineData engineData =
	{
		.appTitle = "Hyperflow",
		// .renderingApi = hf::renderer::GetBestApiType(),
		.renderingApi = hf::RenderingApiType::Vulkan,
		.updateType = hf::EngineUpdateType::Continues,
		.lifecycleCallbacks =
		{
			.onRendererLoad     	 = app::AppRendererLoad,
			.onResourcesLoad     	 = app::AppLoadResources,
			.onStartCallback     	 = app::AppStart,
			.onUpdateCallback    	 = app::AppUpdate,
			.onQuitCallback      	 = app::AppQuit,
		},
		.windowData =
		{
			.title        		 = "Hyperflow",
			.flags        		 = hf::WindowFlags::Default,
			.style        		 = hf::WindowStyle::Default,
			.position     		 = { 200, 200 },
			.size         		 = { 920, 480 },
			.vsyncMode			 = hf::VsyncMode::Relaxed,
			.rnEventInfo =
			{
				.onPassCreationCallback = app::DefineMainPresentationPass,
				.onPreRenderCallback 	= app::AppPreRender,
				.onRenderCallback    	= app::AppRender,
			},
		}
	};

	hf::Run(engineData);
	return 0;
}
