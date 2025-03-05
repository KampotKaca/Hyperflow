#include "hwindow.h"
#include "hplatform.h"

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
		Platform_Initialize();
		log_set_level(LOG_TRACE);

		Hyperflow::s_LifecycleCallbacks = engineData.lifecycleCallbacks;
		Hyperflow::s_UpdateType = engineData.updateType;
		Hyperflow::s_AppTitle = engineData.appTitle;

		s_MainWindow = OpenWindow(engineData.windowData, nullptr);

		Hyperflow::s_IsRunning = true;

		if(s_LifecycleCallbacks.onStartCallback) s_LifecycleCallbacks.onStartCallback();

		while (IsRunning())
		{
			Platform_HandleEvents(s_Windows, s_UpdateType);
			if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();

//			Platform_UpdateTime();
		}

		if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();
		CloseWindow(s_MainWindow);
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
		if(result != Hyperflow::s_Windows.end()) window->Close();
	}

	void SubscribeOnKey(KeySubscriptionData* data) { SubscribeOnKey(Hyperflow::s_MainWindow, data); }
	void UnsubscribeOnKey(KeySubscriptionData* data) { UnsubscribeOnKey(Hyperflow::s_MainWindow, data); }
}