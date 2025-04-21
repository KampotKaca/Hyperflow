#include "hyperflow.h"

#define private public
#include "application.h"
#undef private

static void __attribute__((constructor)) global_init()
{
	hf::Preload();
}

static void __attribute__((destructor)) global_shutdown()
{
	hf::Shutdown();
}

int main()
{
	hf::EngineData engineData =
	{
		.appTitle = "Hyperflow",
		.updateType = hf::EngineUpdateType::Continues,
		.lifecycleCallbacks =
		{
			.onStartCallback    = app::Application::Start,
			.onUpdateCallback   = app::Application::Update,
			.onQuitCallback     = app::Application::Quit,
		},
		.windowData =
		{
			.title     = "Hyperflow",
			.flags     = hf::WindowFlags::Default,
			.style     = hf::WindowStyle::Default,
			.position  = { 200, 200 },
			.size      = { 920, 480 },
			.onRenderCallback = app::Application::OnMainWindowRender,
		}
	};

	hf::Run(engineData);
	return 0;
}
