#include "hyperflow.h"

#define private public
#include "application.h"
#undef private

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
			.onPassCreationCallback  = app::Application::OnPassCreationCallback,
			.onRendererLoad     	 = app::Application::OnRendererLoad,
			.onResourcesLoad     	 = app::Application::LoadResources,
			.onStartCallback     	 = app::Application::Start,
			.onUpdateCallback    	 = app::Application::Update,
			.onQuitCallback      	 = app::Application::Quit,
		},
		.windowData =
		{
			.title        		 = "Hyperflow",
			.flags        		 = hf::WindowFlags::Default,
			.style        		 = hf::WindowStyle::Default,
			.position     		 = { 200, 200 },
			.size         		 = { 920, 480 },
			.onPreRenderCallback = app::Application::OnPreRender,
			.onRenderCallback    = app::Application::OnRender,
		}
	};

	hf::Run(engineData);
	return 0;
}
