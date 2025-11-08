//
// Created by ratib on 08.11.2025.
//

#include "appconfig.h"
#include "hyperflow.h"
#include "application.h"
#include "debug.h"
#include "heditorapplication.h"

namespace app
{
    void EditorRendererLoad()
    {
        AppRendererLoad();
    }

    void EditorLoadResources()
    {
        AppLoadResources();
    }

    void EditorStart()
    {
        AppStart();
    }

    void EditorPreUpdate()
    {
        AppPreUpdate();
    }

    void EditorPostUpdate()
    {
        AppPostUpdate();
    }

    void EditorQuit()
    {
        AppQuit();
    }

    void EditorRendererInit()
    {
        std::array formats = { hf::TextureFormat::R8G8B8A8_Unorm };
        hf::ed::LoadInfo editorInfo
        {
            .windowHandle = hf::GetWindowHandle(),
            .multisampleMode = MSAA_MODE,
            .pColorFormats = formats.data(),
            .colorFormatCount = formats.size(),
            .depthFormat = DEPTH_STENCIL_MODE
        };
        editorInfo.renderApiHandles = hf::GetEditorApiHandles();
        hf::ed::Load(editorInfo);

        AppRendererInit();
    }

    void EditorRendererShutdown()
    {
        AppRendererShutdown();
        hf::ed::Unload();
    }

    void EditorPreRender()
    {
        hf::ed::StartFrame();

        hf::ed::SetNextWindowSize(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        hf::ed::SetNextWindowPos(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        if (hf::ed::StartWindow("Project Settings"))
        {
            if (hf::ed::StartDropdown("Lights"))
            {
                hf::ed::Draw("Directional", APP_OBJECTS.mainLight);
                hf::ed::EndDropdown();
            }

            hf::ed::EndWindow();
        }

        hf::ed::DrawMemoryStatisticsWindow("Memory Statistics");
        hf::ed::DrawRenderStatisticsWindow("Rendering Statistics");
        hf::ed::DrawAudioSettingsWindow("Audio Settings");

        hf::ed::EditorContextData data{};
        data.camera = &APP_DEBUG.camera.camera3D.core;
        data.drawGridLines = &APP_DEBUG.drawGridLines;
        data.gridLines = &APP_DEBUG.gridLinesInfo;
        hf::ed::DrawEditorWindow("Editor", data);

        hf::ed::SetNextWindowSize(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        hf::ed::SetNextWindowPos(hf::vec2{ 100, 100 }, hf::ed::Condition::FirstUseEver);
        if (hf::ed::StartWindow("Inspector"))
        {
            if (hf::ed::StartDropdown("Viking Room"))
            {
                // hf::editor::Draw("Transform", APP_OBJECTS.vikingRoom.transform);
                hf::ed::Draw("Listener", APP_OBJECTS.mainListener);
                hf::ed::EndDropdown();
            }

            if (hf::ed::StartDropdown("Ground"))
            {
                hf::ed::Draw("Transform", APP_OBJECTS.ground.transform);
                hf::ed::EndDropdown();
            }

            if (hf::ed::StartDropdown("Sphere"))
            {
                hf::ed::Draw("Transform", APP_OBJECTS.sphere.transform);
                hf::ed::EndDropdown();
            }

            hf::ed::EndWindow();
        }

        hf::ed::EndFrame();

        AppPreRender();
    }

    void GuiDraw(void* cmd) { hf::ed::DrawFrame(cmd); }

    void EditorStartRender()
    {
        AppStartRender();
    }

    void EditorEndRender()
    {
        hf::dp::SetDrawCallback(GuiDraw);
        AppEndRender();
    }
}