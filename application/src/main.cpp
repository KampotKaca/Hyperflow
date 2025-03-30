#define private public
#include "hyperflow.h"
#include "application.h"
#undef private

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
			.onRenderCallback   = app::Application::Render,
			.onQuitCallback     = app::Application::Quit,
		},
		.windowData =
		{
			.title     = "Hyperflow",
			.flags     = hf::WindowFlags::Default,
			.style     = hf::WindowStyle::Default,
			.position  = { 200, 200 },
			.size      = { 640, 480 },
		}
	};

	hf::Run(engineData);
	return 0;
}
