#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "htime.h"
#include "hwindow.h"
#include "hrenderer.h"
#include "hexception.h"

namespace hf
{
	class Hyperflow
	{
	public:
		static void Run(const EngineData& engineData);
		static bool IsRunning();
		static Ref<Window> MainWindow();

	private:
		static EngineLifecycleCallbacks s_LifecycleCallbacks;
		static EngineUpdateType s_UpdateType;
		static bool s_IsRunning;

		static std::string s_AppTitle;
		static Ref<Window> s_MainWindow;
		static std::vector<Ref<Window>> s_Windows;
	};

	Ref<Window> OpenWindow(const WindowData& data, const Ref<Window>& parent);
	void CloseWindow(const Ref<Window>& window);

	void SubscribeOnKey(KeySubscriptionData* data);
	void UnsubscribeOnKey(KeySubscriptionData* data);
}

#endif //HYPERFLOW_H
