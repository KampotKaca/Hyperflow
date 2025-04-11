#include "hwindow.h"
#include "hyperflow.h"
#include "../../application/appconfig.h"

#include "hplatform.h"
#include "hrenderer.h"
#include "components/htime.h"

#include "hinternal.h"

namespace hf
{
	inter::Hyperflow inter::HF;

	void ClearWindow(const Window* window);

	void Run(const EngineData& engineData)
	{
		try
		{
			inter::HF.time = Time();
			Platform_Initialize();
			Platform_BeginTemporarySystemTimer(1);
			log_set_level(LOG_TRACE);

			inter::HF.lifecycleCallbacks = engineData.lifecycleCallbacks;
			inter::HF.updateType = engineData.updateType;
			inter::HF.appTitle = engineData.appTitle;

			inter::HF.mainWindow = OpenWindow(engineData.windowData, nullptr);

			inter::HF.isRunning = true;

			if(inter::HF.lifecycleCallbacks.onStartCallback) inter::HF.lifecycleCallbacks.onStartCallback();

			LOG_LOG("Loading Time: %f", inter::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				inter::HF.time.StartFrame();

				Platform_HandleEvents(inter::HF.updateType);
				Window_HandleInput(inter::HF.windows);
				if(inter::HF.lifecycleCallbacks.onUpdateCallback) inter::HF.lifecycleCallbacks.onUpdateCallback();

				for(auto& window : inter::HF.windows)
				{
					if (!window::IsClosing(window)) inter::window::Update(window.get());
				}

				if(input::IsDown(Key::Escape)) Terminate();
			}
			if(inter::HF.lifecycleCallbacks.onUpdateCallback) inter::HF.lifecycleCallbacks.onQuitCallback();

			for (const auto& window : inter::HF.windows) inter::window::Close(window.get());
			inter::HF.windows.clear();
			Platform_EndTemporarySystemTimer(1);
			Platform_Dispose();
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

	Ref<Window> OpenWindow(const WindowData &data, const Ref<Window> &parent)
	{
		auto newWindow = MakeRef<Window>(data, parent);
		newWindow->renderer = MakeURef<Renderer>(newWindow, APP_VERSION);
		inter::HF.windows.push_back(newWindow);
		return newWindow;
	}

	void Terminate() { inter::HF.isRunning = false; }

	void ClearWindow(const Window* window)
	{
		std::erase_if(inter::HF.windows, [&](Ref<Window> &w)
		{
			return w.get() == window;
		});
	}
}