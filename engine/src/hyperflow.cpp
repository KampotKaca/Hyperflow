#include "hwindow.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "htime.h"
#include "hinternal.h"
#include "hscene.h"

namespace hf
{
	ir::Hyperflow ir::HF;

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
			ir::HF.internalResourcesFormat = engineData.internalResourcesFormat;
			ir::HF.appTitle = engineData.appTitle;

			ir::HF.window = MakeURef<Window>(engineData.windowData);

			if (ir::HF.internalAudioInfo.audioEnabled) ir::audio::Load_i();

			ir::rdr::LoadApi_i(engineData.renderingApi);
			ir::general::LoadStaticResources_i();
			if(ir::HF.lifecycleCallbacks.onResourcesLoad) ir::HF.lifecycleCallbacks.onResourcesLoad();
			ir::HF.isRunning = true;

			if(ir::HF.lifecycleCallbacks.onStartCallback) ir::HF.lifecycleCallbacks.onStartCallback();

			log_info_s("[Hyperflow] Loading Time: %f", ir::HF.time.GetAbsoluteTimePassed());
			while (IsRunning())
			{
				ir::HF.time.StartFrame();
				ir::platform::HandleEvents();
				if (!IsRunning()) continue;

			    if(ir::HF.lifecycleCallbacks.onPreUpdateCallback) ir::HF.lifecycleCallbacks.onPreUpdateCallback();
                for (const auto& scene : ir::HF.scenes | std::views::values) scene->Update();
			    if(ir::HF.lifecycleCallbacks.onPostUpdateCallback) ir::HF.lifecycleCallbacks.onPostUpdateCallback();

				if(IsKeyDown(Key::Escape)) Terminate();
			    if (!IsRunning()) continue;

                if (!IsWindowClosed())
                {
                    ir::rdr::StartRenderPacket_i();
                    const auto& cInfo = ir::HF.renderer->eventInfo;
                    if (cInfo.onPreRenderCallback) cInfo.onPreRenderCallback();
                    ir::rdr::PreDraw_i();
                    {
                        if (cInfo.onRenderStartCallback) cInfo.onRenderStartCallback();
                        for (const auto& scene : ir::HF.scenes | std::views::values) scene->Render();
                        if (cInfo.onRenderEndCallback) cInfo.onRenderEndCallback();
                    }
                    ir::rdr::EndRenderPacket_i();
                }
			}
			if (ir::HF.lifecycleCallbacks.onQuitCallback) ir::HF.lifecycleCallbacks.onQuitCallback();

			UnloadAllScenes();
			ir::audio::Unload_i();
		    if (!IsWindowClosed()) ir::win::Close(ir::HF.window.get());
			ir::platform::Unload();
		}
		catch(const HyperException& e)
		{
			log_fatal_i(e.GetFile().c_str(), e.GetLine(), e.what());
		}
		catch(const std::exception& e)
		{
			log_fatal(e.what());
		}
		catch(...)
		{
			log_fatal("No Details Are Available");
		}
	}

	bool IsRunning() { return ir::HF.isRunning && !IsWindowClosed(); }
	const std::string& GetApplicationTitle() { return ir::HF.appTitle; }
	void Terminate() { ir::HF.isRunning = false; }

    void* GetEditorApiHandles()
	{
	    return ir::HF.renderingApi.api.GetEditorInfo();
	}
}