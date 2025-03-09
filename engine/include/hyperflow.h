#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "components/htime.h"
#include "components/windowhandling/hwindow.h"
#include "rendering/hrenderer.h"
#include "hexception.h"
#include "hinput.h"

namespace hf
{
	class Hyperflow
	{
	public:
		static void Run(const EngineData& engineData);
		static bool IsRunning();
		static Ref<Window> MainWindow();

		static Ref<Window> OpenWindow(const WindowData& data, const Ref<Window>& parent);
		static void CloseWindow(const Ref<Window>& window);

		static void Terminate();

	private:
		static EngineLifecycleCallbacks s_LifecycleCallbacks;
		static EngineUpdateType s_UpdateType;
		static bool s_IsRunning;

		static std::string s_AppTitle;
		static Ref<Window> s_MainWindow;
		static std::vector<Ref<Window>> s_Windows;
	};
}

#endif //HYPERFLOW_H
