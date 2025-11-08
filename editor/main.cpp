//
// Created by ratib on 08.11.2025.
//

#include "hyperflow.h"
#include "application.h"
#include "heditorapplication.h"
#include "appconfig.h"

int main()
{
#if !DEBUG
    freopen("log.txt", "w", stdout);
    freopen("log.txt", "w", stderr);
#endif

    hf::EngineData engineData =
    {
        .appTitle = "Hyperflow",
        .renderingApi = hf::RenderingApiType::Vulkan,
        .internalResourcesFormat = hf::EngineInternalResourceFormatInfo
        {
            .drawOutputFormats = hf::ShaderDrawOutputFormats
            {
                .sampleMode  = MSAA_MODE,
                .depthFormat = DEPTH_STENCIL_MODE,
            },
        },
        .lifecycleCallbacks = hf::EngineLifecycleCallbacks
        {
            .onRendererLoad   = app::EditorRendererLoad,
            .onResourcesLoad  = app::EditorLoadResources,
            .onStartCallback  = app::EditorStart,
            .onPreUpdateCallback  = app::EditorPreUpdate,
            .onPostUpdateCallback = app::EditorPostUpdate,
            .onQuitCallback   = app::EditorQuit,
        },
        .audioInfo = hf::EngineInternalAudioInfo
        {
            .audioEnabled = true,
            .volume = 1.0f,
            .usedListenersCount = 1
        },
        .windowData = hf::WindowCreationInfo
        {
            .title        	= "Hyperflow",
            .iconFolderPath	= "app/icons",
            .eventFlags		= hf::WindowEventFlags::Default,
            .pointerState	= hf::WindowPointerState::Normal,
            .style        	= hf::WindowStyle::Default,
            .position     	= { 200, 200 },
            .size         	= { 920, 480 },
            .vsyncMode		= hf::VsyncMode::Relaxed,
            .rnEventInfo = hf::RendererEventInfo
            {
                .onRendererInitCallback 	= app::EditorRendererInit,
                .onRendererShutdownCallback = app::EditorRendererShutdown,
                .onPreRenderCallback 	    = app::EditorPreRender,
                .onRenderStartCallback    	= app::EditorStartRender,
                .onRenderEndCallback    	= app::EditorEndRender,
            },
        }
    };

    hf::Run(engineData);
    return 0;
}