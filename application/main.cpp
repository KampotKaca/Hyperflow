#include "hyperflow.h"
#include "application.h"

int main()
{
#if !DEBUG
	freopen("log.txt", "w", stdout);
	freopen("log.txt", "w", stderr);
#endif

	hf::EngineData engineData =
	{
		.appTitle = "Hyperflow",
		.renderingApi = hf::RenderingApiType::Vulkan,
	    .internalResourcesFormat = hf::EngineInternalResourceFormatInfo
	    {
		    .drawOutputFormats = hf::ShaderDrawOutputFormats
		    {
		        .sampleMode  = MSAA_MODE,
		        .depthFormat = DEPTH_STENCIL_MODE,
		    },
	    },
		.lifecycleCallbacks = hf::EngineLifecycleCallbacks
		{
			.onRendererLoad   = app::AppRendererLoad,
			.onResourcesLoad  = app::AppLoadResources,
			.onStartCallback  = app::AppStart,
			.onPreUpdateCallback  = app::AppPreUpdate,
			.onPostUpdateCallback = app::AppPostUpdate,
			.onQuitCallback   = app::AppQuit,
		},
	    .audioInfo = hf::EngineInternalAudioInfo
	    {
	        .audioEnabled = true,
		    .volume = 1.0f,
		    .usedListenersCount = 1
	    },
		.windowData = hf::WindowCreationInfo
		{
			.title        	= "Hyperflow",
			.iconFolderPath	= "app/icons",
			.eventFlags		= hf::WindowEventFlags::Default,
			.pointerState	= hf::WindowPointerState::Normal,
			.style        	= hf::WindowStyle::Default,
			.position     	= { 200, 200 },
			.size         	= { 920, 480 },
			.vsyncMode		= hf::VsyncMode::Relaxed,
			.rnEventInfo = hf::RendererEventInfo
			{
				.onRendererInitCallback 	= app::AppRendererInit,
				.onRendererShutdownCallback = app::AppRendererShutdown,
				.onPreRenderCallback 	    = app::AppPreRender,
				.onRenderStartCallback    	= app::AppStartRender,
				.onRenderEndCallback    	= app::AppEndRender,
			},
		}
	};

	hf::Run(engineData);
	return 0;
}
