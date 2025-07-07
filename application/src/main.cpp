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
		        .sampleMode = MSAA_MODE,
		        .depthFormat = DEPTH_STENCIL_MODE,
		    }
	    },
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
			.iconFolderPath		 = "app/icons",
			.state				 = hf::WindowState::Restored,
			.eventFlags			 = hf::WindowEventFlags::Default,
			.pointerState		 = hf::WindowPointerState::Disabled,
			.style        		 = hf::WindowStyle::Default,
			.position     		 = { 200, 200 },
			.size         		 = { 920, 480 },
			.vsyncMode			 = hf::VsyncMode::Relaxed,
			.rnEventInfo =
			{
				.onPreRenderCallback 	= app::AppPreRender,
				.onRenderCallback    	= app::AppRender,
			},
		}
	};

	hf::Run(engineData);
	return 0;
}
