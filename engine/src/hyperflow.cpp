#include "hyperflow.h"
#include "hwindow.h"
#include "hplatform.h"

namespace hf
{
	EngineLifecycleCallbacks Hyperflow::s_LifecycleCallbacks;
	EngineUpdateType Hyperflow::s_UpdateType;
	bool Hyperflow::s_IsRunning;

	std::string Hyperflow::s_AppTitle;
	hRef<Window> Hyperflow::s_MainWindow = NULL;
	std::vector<hRef<Window>> Hyperflow::s_Windows;

	void Hyperflow::Run(const EngineData& engineData)
	{
		Platform_Initialize();
		log_set_level(LOG_TRACE);

		Hyperflow::s_LifecycleCallbacks = engineData.lifecycleCallbacks;
		Hyperflow::s_UpdateType = engineData.updateType;
		Hyperflow::s_AppTitle = engineData.appTitle;

		s_MainWindow = hMakeRef<Window>(engineData.windowData, nullptr);
		s_Windows.push_back(s_MainWindow);

		Hyperflow::s_IsRunning = true;

		if(s_LifecycleCallbacks.onStartCallback) s_LifecycleCallbacks.onStartCallback();

		while (IsRunning())
		{
			Platform_HandleEvents(s_Windows, s_UpdateType);
			if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();

//			Platform_UpdateTime();
		}

		if(s_LifecycleCallbacks.onUpdateCallback) s_LifecycleCallbacks.onUpdateCallback();
		s_MainWindow->Close();
	}

	bool Hyperflow::IsRunning() { return Hyperflow::s_IsRunning && !s_MainWindow->IsClosing(); }
}