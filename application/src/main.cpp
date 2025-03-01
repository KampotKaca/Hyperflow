#include "hyperflow.h"

int main()
{
	hf::EngineData engineData =
	{
		.appTitle = "Hyperflow",
		.updateType = hf::EngineUpdateType::Continues,
		.lifecycleCallbacks =
		{
			.onStartCallback    = NULL,
			.onUpdateCallback   = NULL,
			.onRenderCallback   = NULL,
			.onQuitCallback     = NULL,
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

	hf::Hyperflow::Run(engineData);
	log_warn("You have been warned");
	return 0;
}
