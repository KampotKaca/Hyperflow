#include "hwindow.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "htime.h"
#include "hinternal.h"
#include "hscene.h"

namespace hf
{
	inter::Hyperflow inter::HF;

	void Preload()
	{

	}

	void Shutdown()
	{
		inter::alloc::UnloadAllocator_i();
	}

	void Run(const EngineData& engineData)
	{
		try
		{
			inter::HF.time = Time();
			inter::platform::Load();

			log_set_level(LOG_TRACE);

			{
				using namespace std::filesystem;
				auto target = current_path().parent_path() / "res";

				if (exists(target)) current_path(target);
			}

			inter::HF.lifecycleCallbacks = engineData.lifecycleCallbacks;
			inter::HF.internalAudioInfo = engineData.audioInfo;
			inter::HF.updateType = engineData.updateType;
			inter::HF.internalResourcesFormat = engineData.internalResourcesFormat;
			inter::HF.appTitle = engineData.appTitle;

			inter::HF.mainWindow = Create(engineData.windowData, nullptr);

			if (inter::HF.internalAudioInfo.audioEnabled) inter::audio::Load_i();

			inter::rendering::LoadApi_i(engineData.renderingApi);
			inter::primitives::LoadStaticResources_i();
			if(inter::HF.lifecycleCallbacks.onResourcesLoad) inter::HF.lifecycleCallbacks.onResourcesLoad();
			inter::HF.isRunning = true;

			if(inter::HF.lifecycleCallbacks.onStartCallback) inter::HF.lifecycleCallbacks.onStartCallback();

			LOG_LOG("[Hyperflow] Loading Time: %f", inter::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				inter::HF.time.StartFrame();
				inter::platform::HandleEvents(inter::HF.updateType);
				if (!IsRunning()) continue;

			    if(inter::HF.lifecycleCallbacks.onPreUpdateCallback) inter::HF.lifecycleCallbacks.onPreUpdateCallback();
                for (const auto& scene : inter::HF.scenes | std::views::values) scene->Update();
			    if(inter::HF.lifecycleCallbacks.onPostUpdateCallback) inter::HF.lifecycleCallbacks.onPostUpdateCallback();

			    if (!IsRunning()) continue;
				if(IsKeyDown(Key::Escape))
				{
					Terminate();
				    continue;
				}

				inter::HF.tempWindows.clear();
				for (auto& window : inter::HF.windows | std::views::values)
					inter::HF.tempWindows.push_back(window);

				for(auto& window : inter::HF.tempWindows)
				{
					if (!IsClosed(window))
					{
						auto rn = window->renderer;
						inter::rendering::StartRenderPacket_i(rn);
						const auto& cInfo = rn->eventInfo;
						if (cInfo.onPreRenderCallback) cInfo.onPreRenderCallback(rn);
						inter::rendering::PreDraw_i(rn);
						{
						    if (cInfo.onRenderStartCallback) cInfo.onRenderStartCallback(rn);
						    for (const auto& scene : inter::HF.scenes | std::views::values) scene->Render(rn);
						    if (cInfo.onRenderEndCallback) cInfo.onRenderEndCallback(rn);
						}
						inter::rendering::EndRenderPacket_i(rn);
					}
				}
			}
			if (inter::HF.lifecycleCallbacks.onQuitCallback) inter::HF.lifecycleCallbacks.onQuitCallback();

			UnloadAllScenes();
			DestroyAllWindows();
			inter::audio::Unload_i();
			inter::platform::Unload();
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

	bool IsRunning() { return inter::HF.isRunning && !IsClosed(inter::HF.mainWindow); }

	Ref<Window> GetMainWindow() { return inter::HF.mainWindow; }
	const std::string& GetApplicationTitle() { return inter::HF.appTitle; }

	void Terminate() { inter::HF.isRunning = false; }

    void* GetEditorApiHandles()
	{
	    return inter::HF.renderingApi.api.GetEditorInfo();
	}
}