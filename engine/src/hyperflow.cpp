#define private public
#include "hwindow.h"
#undef private

#include "hyperflow.h"
#include "hplatform.h"
#include "components/hinternal.h"
#include "hrenderer.h"

namespace hf
{
	EngineLifecycleCallbacks Hyperflow::s_LifecycleCallbacks;
	EngineUpdateType Hyperflow::s_UpdateType;
	bool Hyperflow::s_IsRunning;

	std::string Hyperflow::s_AppTitle;
	Ref<Window> Hyperflow::s_MainWindow = nullptr;
	std::vector<Ref<Window>> Hyperflow::s_Windows;
	void* Hyperflow::s_PlatformHandle = nullptr;

	void Hyperflow::Run(const EngineData& engineData)
	{
		try
		{
			Time_Load();
			s_PlatformHandle = Platform_Initialize();
			Platform_BeginTemporarySystemTimer(1);
			log_set_level(LOG_TRACE);

			s_LifecycleCallbacks = engineData.lifecycleCallbacks;
			s_UpdateType = engineData.updateType;
			s_AppTitle = engineData.appTitle;

			s_MainWindow = OpenWindow(engineData.windowData, nullptr);

			s_IsRunning = true;

			if(s_LifecycleCallbacks.onStartCallback) s_LifecycleCallbacks.onStartCallback();

			LOG_INFO("Loading Time: %f", hf::Time::GetAbsoluteTimePassed());
			while (IsRunning())
			{
				Time_Update();

				Platform_HandleEvents(s_UpdateType);
				Window_HandleInput(s_Windows);
				if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();

				for(auto& window : s_Windows)
				{
					if (!window->IsClosing()) window->Update();
				}

				if(Input::IsDown(Key::Escape)) Terminate();
			}
			if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onQuitCallback();

			for (auto window : s_Windows) window->Close();
			s_Windows.clear();
			Platform_EndTemporarySystemTimer(1);
			Platform_Dispose(s_PlatformHandle);
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

	bool Hyperflow::IsRunning() { return s_IsRunning && !s_MainWindow->IsClosing(); }

	Ref<Window> Hyperflow::MainWindow() { return s_MainWindow; }

	Ref<Window> Hyperflow::OpenWindow(const WindowData &data, const Ref<Window> &parent)
	{
		auto newWindow = MakeRef<Window>(s_PlatformHandle, data, parent);
		newWindow->m_Renderer = MakeRef<Renderer>(newWindow);
		s_Windows.push_back(newWindow);
		return newWindow;
	}

	void Hyperflow::Terminate() { s_IsRunning = false; }
	void* Hyperflow::GetPlatformHandle() { return s_PlatformHandle; }
	void Hyperflow::ClearWindow(Window* window)
	{
		std::erase_if(s_Windows, [&](Ref<Window> &w)
		{
			return w.get() == window;
		} );
	}
}