#include "hwindow.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "htime.h"
#include "hinternal.h"
#include "hscene.h"

namespace hf
{
	ir::Hyperflow ir::HF;

	void Preload()
	{

	}

	void Shutdown()
	{
		ir::alloc::UnloadAllocator_i();
	}

	void Run(const EngineData& engineData)
	{
		try
		{
			ir::HF.time = Time();
			ir::platform::Load();

			log_set_level(LOG_TRACE);

			{
				using namespace std::filesystem;
				auto target = current_path().parent_path() / "res";

				if (exists(target)) current_path(target);
			}

			ir::HF.lifecycleCallbacks = engineData.lifecycleCallbacks;
			ir::HF.internalAudioInfo = engineData.audioInfo;
			ir::HF.updateType = engineData.updateType;
			ir::HF.internalResourcesFormat = engineData.internalResourcesFormat;
			ir::HF.appTitle = engineData.appTitle;

			ir::HF.mainWindow = Create(engineData.windowData, nullptr);

			if (ir::HF.internalAudioInfo.audioEnabled) ir::audio::Load_i();

			ir::rdr::LoadApi_i(engineData.renderingApi);
			ir::general::LoadStaticResources_i();
			if(ir::HF.lifecycleCallbacks.onResourcesLoad) ir::HF.lifecycleCallbacks.onResourcesLoad();
			ir::HF.isRunning = true;

			if(ir::HF.lifecycleCallbacks.onStartCallback) ir::HF.lifecycleCallbacks.onStartCallback();

			LOG_LOG("[Hyperflow] Loading Time: %f", ir::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				ir::HF.time.StartFrame();
				ir::platform::HandleEvents(ir::HF.updateType);
				if (!IsRunning()) continue;

			    if(ir::HF.lifecycleCallbacks.onPreUpdateCallback) ir::HF.lifecycleCallbacks.onPreUpdateCallback();
                for (const auto& scene : ir::HF.scenes | std::views::values) scene->Update();
			    if(ir::HF.lifecycleCallbacks.onPostUpdateCallback) ir::HF.lifecycleCallbacks.onPostUpdateCallback();

			    if (!IsRunning()) continue;
				if(IsKeyDown(Key::Escape))
				{
					Terminate();
				    continue;
				}

				ir::HF.tempWindows.clear();
				for (auto& window : ir::HF.windows | std::views::values)
					ir::HF.tempWindows.push_back(window);

				for(auto& window : ir::HF.tempWindows)
				{
					if (!IsClosed(window))
					{
						auto rn = window->renderer;
						ir::rdr::StartRenderPacket_i(rn);
						const auto& cInfo = rn->eventInfo;
						if (cInfo.onPreRenderCallback) cInfo.onPreRenderCallback(rn);
						ir::rdr::PreDraw_i(rn);
						{
						    if (cInfo.onRenderStartCallback) cInfo.onRenderStartCallback(rn);
						    for (const auto& scene : ir::HF.scenes | std::views::values) scene->Render(rn);
						    if (cInfo.onRenderEndCallback) cInfo.onRenderEndCallback(rn);
						}
						ir::rdr::EndRenderPacket_i(rn);
					}
				}
			}
			if (ir::HF.lifecycleCallbacks.onQuitCallback) ir::HF.lifecycleCallbacks.onQuitCallback();

			UnloadAllScenes();
			DestroyAllWindows();
			ir::audio::Unload_i();
			ir::platform::Unload();
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

	bool IsRunning() { return ir::HF.isRunning && !IsClosed(ir::HF.mainWindow); }

	Ref<Window> GetMainWindow() { return ir::HF.mainWindow; }
	const std::string& GetApplicationTitle() { return ir::HF.appTitle; }

	void Terminate() { ir::HF.isRunning = false; }

    void* GetEditorApiHandles()
	{
	    return ir::HF.renderingApi.api.GetEditorInfo();
	}
}