#include "hwindow.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "htime.h"
#include "hinternal.h"

namespace hf
{
	inter::Hyperflow inter::HF;

	void Preload()
	{

	}

	void Shutdown()
	{
		inter::alloc::UnloadAllocator_i();
	}

	void Run(const EngineData& engineData)
	{
		try
		{
			inter::HF.time = Time();
			inter::platform::Load();

			log_set_level(LOG_TRACE);

			inter::HF.lifecycleCallbacks = engineData.lifecycleCallbacks;
			inter::HF.updateType = engineData.updateType;
			inter::HF.appTitle = engineData.appTitle;

			inter::HF.mainWindow = window::Open(engineData.windowData, nullptr);

			inter::rendering::LoadApi_i(engineData.renderingApi);
			if(inter::HF.lifecycleCallbacks.onResourcesLoad) inter::HF.lifecycleCallbacks.onResourcesLoad();
			inter::HF.renderingApi.api.SubmitBufferCopyOperations();
			inter::HF.renderingApi.api.SubmitTextureCopyOperations();
			inter::HF.isRunning = true;

			if(inter::HF.lifecycleCallbacks.onStartCallback) inter::HF.lifecycleCallbacks.onStartCallback();

			LOG_LOG("Loading Time: %f", inter::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				inter::HF.time.StartFrame();
				inter::platform::HandleEvents(inter::HF.updateType);
				Window_HandleInput(inter::HF.windows);
				if(inter::HF.lifecycleCallbacks.onUpdateCallback) inter::HF.lifecycleCallbacks.onUpdateCallback();

				if(input::IsDown(Key::Escape))
				{
					inter::alloc::LogThreadMemoryStats_i();
					Terminate();
				}

				for(auto& window : inter::HF.windows)
				{
					if (!window::IsClosing(window)) inter::window::Update(window.get());
				}
			}
			if (inter::HF.renderingApi.isLoaded) inter::HF.renderingApi.api.WaitForRendering();
			if (inter::HF.lifecycleCallbacks.onQuitCallback) inter::HF.lifecycleCallbacks.onQuitCallback();

			window::CloseAll();
			inter::platform::Unload();
		}
		catch(const HyperException& e)
		{
			log_fatal(e.GetFile().c_str(), e.GetLine(), e.what());
		}
		catch(const std::exception& e)
		{
			LOG_FATAL(e.what());
		}
		catch(...)
		{
			LOG_FATAL("No Details Are Available");
		}
	}

	bool IsRunning() { return inter::HF.isRunning && !window::IsClosing(inter::HF.mainWindow); }

	Ref<Window> GetMainWindow() { return inter::HF.mainWindow; }
	const std::string& GetApplicationTitle() { return inter::HF.appTitle; }

	void Terminate() { inter::HF.isRunning = false; }
}