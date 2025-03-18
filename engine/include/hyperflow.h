#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"
#include "components/htime.h"
#include "hwindow.h"
#include "hrenderer.h"
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
		static void Terminate();
		static void* GetPlatformHandle();

	private:
		static EngineLifecycleCallbacks s_LifecycleCallbacks;
		static EngineUpdateType s_UpdateType;
		static bool s_IsRunning;

		static std::string s_AppTitle;
		static Ref<Window> s_MainWindow;
		static std::vector<Ref<Window>> s_Windows;
		static void* s_PlatformHandle;

		static void ClearWindow(Window* window);
	};
}

#endif //HYPERFLOW_H
