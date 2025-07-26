#include "hyperflow.h"
#include "application.h"
#include "resources.h"
#include "../appconfig.h"

static void __attribute__((constructor)) global_init()
{
	hf::Preload();
}

int main()
{
#if !DEBUG
	freopen("log.txt", "w", stdout);
	freopen("log.txt", "w", stderr);
#endif

	hf::EngineData engineData =
	{
		.appTitle = "Hyperflow",
		// .renderingApi = hf::renderer::GetBestApiType(),
		.renderingApi = hf::RenderingApiType::Vulkan,
		.updateType = hf::EngineUpdateType::Continues,
	    .internalResourcesFormat =
	    {
		    .globalUniformBindingInfo =
        {
		        .usageFlags = hf::ShaderUsageStage::All,
                .arraySize = 1,
                .elementSizeInBytes = sizeof(app::GlobalUniformInfo)
            },
		    .drawOutputFormats =
		    {
		        .depthFormat = DEPTH_STENCIL_MODE,
		    },
		    .samplingMode = MSAA_MODE
	    },
		.lifecycleCallbacks =
		{
			.onRendererLoad     	 = app::AppRendererLoad,
			.onResourcesLoad     	 = app::AppLoadResources,
			.onStartCallback     	 = app::AppStart,
			.onUpdateCallback    	 = app::AppUpdate,
			.onQuitCallback      	 = app::AppQuit,
		},
	    .audioInfo =
	    {
		    .volume = 1.0f,
		    .usedListenersCount = 1
	    },
		.windowData =
		{
			.title        		 = "Hyperflow",
			.iconFolderPath		 = "app/icons",
			.eventFlags			 = hf::WindowEventFlags::Default,
			.pointerState		 = hf::WindowPointerState::Normal,
			.style        		 = hf::WindowStyle::Default,
			.position     		 = { 200, 200 },
			.size         		 = { 920, 480 },
			.vsyncMode			 = hf::VsyncMode::Relaxed,
			.rnEventInfo =
			{
				.onRendererInitCallback 	= app::AppRendererInit,
				.onRendererShutdownCallback = app::AppRendererShutdown,
				.onPreRenderCallback 	= app::AppPreRender,
				.onRenderCallback    	= app::AppRender,
			},
		}
	};

	hf::Run(engineData);
	return 0;
}
