#define HF_ENGINE_INTERNALS
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
			inter::HF.globalUniformBindingInfo = engineData.globalUniformBindingInfo;
			inter::HF.appTitle = engineData.appTitle;

			inter::HF.mainWindow = Window::Open(engineData.windowData, nullptr);

			inter::rendering::LoadApi_i(engineData.renderingApi);
			inter::primitives::LoadStaticResources_i();
			if(inter::HF.lifecycleCallbacks.onResourcesLoad) inter::HF.lifecycleCallbacks.onResourcesLoad();
			inter::HF.isRunning = true;

			if(inter::HF.lifecycleCallbacks.onStartCallback) inter::HF.lifecycleCallbacks.onStartCallback();

			LOG_LOG("Loading Time: %f", inter::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				inter::HF.time.StartFrame();
				inter::platform::HandleEvents(inter::HF.updateType);
				if (!IsRunning()) break;
				if(inter::HF.lifecycleCallbacks.onUpdateCallback) inter::HF.lifecycleCallbacks.onUpdateCallback();

				if(input::IsDown(Key::Escape))
				{
					inter::alloc::LogThreadMemoryStats_i();
					Terminate();
				}

				inter::HF.tempWindows.clear();
				for (auto& window : inter::HF.windows | std::views::values)
					inter::HF.tempWindows.push_back(window);

				for(auto& window : inter::HF.tempWindows)
				{
					if (!window->IsClosed())
					{
						auto rn = window->renderer;
						inter::rendering::StartRenderPacket_i(rn);
						auto& cInfo = rn->eventInfo;
						if (cInfo.onPreRenderCallback) cInfo.onPreRenderCallback(rn);
						if (cInfo.onRenderCallback) cInfo.onRenderCallback(rn);
						inter::rendering::EndRenderPacket_i(rn);
					}
				}
			}
			if (inter::HF.lifecycleCallbacks.onQuitCallback) inter::HF.lifecycleCallbacks.onQuitCallback();

			Window::CloseAll();
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

	bool IsRunning() { return inter::HF.isRunning && !inter::HF.mainWindow->IsClosed(); }

	Ref<Window> GetMainWindow() { return inter::HF.mainWindow; }
	const std::string& GetApplicationTitle() { return inter::HF.appTitle; }

	void Terminate() { inter::HF.isRunning = false; }
}