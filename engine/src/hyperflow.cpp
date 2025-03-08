#include "hplatform.h"
#include "hinternal.h"

#define private public
#include "hwindow.h"
#include "hyperflow.h"
#undef private

namespace hf
{
	EngineLifecycleCallbacks Hyperflow::s_LifecycleCallbacks;
	EngineUpdateType Hyperflow::s_UpdateType;
	bool Hyperflow::s_IsRunning;

	std::string Hyperflow::s_AppTitle;
	Ref<Window> Hyperflow::s_MainWindow = nullptr;
	std::vector<Ref<Window>> Hyperflow::s_Windows;

	void Hyperflow::Run(const EngineData& engineData)
	{
		try
		{
			Platform_Initialize();
			Platform_BeginTemporarySystemTimer(1);
			LoadTime();
			log_set_level(LOG_TRACE);

			Hyperflow::s_LifecycleCallbacks = engineData.lifecycleCallbacks;
			Hyperflow::s_UpdateType = engineData.updateType;
			Hyperflow::s_AppTitle = engineData.appTitle;

			s_MainWindow = OpenWindow(engineData.windowData, nullptr);

			Hyperflow::s_IsRunning = true;

			if(s_LifecycleCallbacks.onStartCallback) s_LifecycleCallbacks.onStartCallback();

			while (IsRunning())
			{
				UpdateTime();
				Platform_HandleEvents(s_Windows, s_UpdateType);
				if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();
			}

			if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();
			CloseWindow(s_MainWindow);
			Platform_EndTemporarySystemTimer(1);
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

	bool Hyperflow::IsRunning() { return Hyperflow::s_IsRunning && !s_MainWindow->IsClosing(); }

	Ref<Window> Hyperflow::MainWindow() { return s_MainWindow; }

	Ref<Window> OpenWindow(const WindowData &data, const Ref<Window>& parent)
	{
		auto newWindow = MakeRef<Window>(data, parent);
		Hyperflow::s_Windows.push_back(newWindow);
		return newWindow;
	}

	void CloseWindow(const Ref<Window>& window)
	{
		auto result = std::remove(Hyperflow::s_Windows.begin(), Hyperflow::s_Windows.end(), window);
		if(result != Hyperflow::s_Windows.end()) window->m_ShouldClose = true;
	}

	void SubscribeOnKey(KeySubscriptionData* data) { SubscribeOnKey(Hyperflow::s_MainWindow, data); }
	void UnsubscribeOnKey(KeySubscriptionData* data) { UnsubscribeOnKey(Hyperflow::s_MainWindow, data); }
}