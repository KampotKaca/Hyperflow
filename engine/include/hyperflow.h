#ifndef HYPERFLOW_H
#define HYPERFLOW_H

#include "hshared.h"

namespace hf
{
	class Hyperflow
	{
	public:
		static void Run(const EngineData& engineData);
		static bool IsRunning();
	private:
		static EngineLifecycleCallbacks s_LifecycleCallbacks;
		static EngineUpdateType s_UpdateType;
		static bool s_IsRunning;

		static std::string s_AppTitle;
		static hRef<Window> s_MainWindow;
		static std::vector<hRef<Window>> s_Windows;
	};
}

#endif //HYPERFLOW_H
