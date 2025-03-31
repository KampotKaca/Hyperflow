#define private public
#include "hwindow.h"
#undef private

#include "hyperflow.h"
#include "../../application/appconfig.h"

#include "hplatform.h"
#include "hrenderer.h"
#include "components/htime.h"
#include <sstream>

namespace hf
{
	struct Hyperflow
	{
		EngineLifecycleCallbacks lifecycleCallbacks;
		EngineUpdateType updateType;
		bool isRunning;
		std::string appTitle;
		Time time;
		Ref<Window> mainWindow;
		std::vector<Ref<Window>> windows;
	};

	void ClearWindow(const Window* window);

	Hyperflow HF;

	void Run(const EngineData& engineData)
	{
		try
		{
			HF.time = Time();
			Platform_Initialize();
			Platform_BeginTemporarySystemTimer(1);
			log_set_level(LOG_TRACE);

			HF.lifecycleCallbacks = engineData.lifecycleCallbacks;
			HF.updateType = engineData.updateType;
			HF.appTitle = engineData.appTitle;

			HF.mainWindow = OpenWindow(engineData.windowData, nullptr);

			HF.isRunning = true;

			if(HF.lifecycleCallbacks.onStartCallback) HF.lifecycleCallbacks.onStartCallback();

			LOG_INFO("Loading Time: %f", HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				HF.time.Update();

				Platform_HandleEvents(HF.updateType);
				Window_HandleInput(HF.windows);
				if(HF.lifecycleCallbacks.onUpdateCallback) HF.lifecycleCallbacks.onUpdateCallback();

				for(auto& window : HF.windows)
				{
					if (!window->IsClosing()) window->Update();
				}

				if(input::IsDown(Key::Escape)) Terminate();
			}
			if(HF.lifecycleCallbacks.onUpdateCallback) HF.lifecycleCallbacks.onQuitCallback();

			for (const auto& window : HF.windows) window->Close();
			HF.windows.clear();
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

	bool IsRunning() { return HF.isRunning && !HF.mainWindow->IsClosing(); }

	Ref<Window> GetMainWindow() { return HF.mainWindow; }
	const std::string& GetApplicationTitle() { return HF.appTitle; }

	Ref<Window> OpenWindow(const WindowData &data, const Ref<Window> &parent)
	{
		auto newWindow = MakeRef<Window>(data, parent);
		newWindow->m_Renderer = MakeRef<Renderer>(newWindow, APP_VERSION);
		HF.windows.push_back(newWindow);
		return newWindow;
	}

	void Terminate() { HF.isRunning = false; }

	void ClearWindow(const Window* window)
	{
		std::erase_if(HF.windows, [&](Ref<Window> &w)
		{
			return w.get() == window;
		});
	}

	namespace time
	{
		uint64_t GetFrameCount() { return HF.time.GetFrameCount(); }
		double GetDeltaTime(){ return HF.time.GetDeltaTime(); }
		double GetTimePassed() { return HF.time.GetTimePassed(); }
		double GetAbsoluteTimePassed() { return HF.time.GetAbsoluteTimePassed(); }
		int16_t GetTargetFrameRate() { return HF.time.GetTargetFrameRate(); }
		int32_t GetFrameRate() { return HF.time.GetFrameRate(); }
		void SetTargetFrameRate(int16_t targetFrameRate) { return HF.time.SetTargetFrameRate(targetFrameRate); }

		double GetSystemTime()
		{
			using namespace std::chrono;
			const auto time = high_resolution_clock::now().time_since_epoch();
			return duration<double>(time).count();
		}
	}

	namespace utils
	{
		ivec3 ConvertVersion(const char* version)
		{
			std::istringstream ss(version);
			std::string token;

			ivec3 v{};
			uint32_t id = 0;
			while (std::getline(ss, token, '.') && id < 3)
			{
				v[id] = std::stoi(token);
				id++;
			}

			return v;
		}
	}
}