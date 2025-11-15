//
// Created by ratib on 15.11.2025.
//

#include "heditorimpl.h"
#include "application.h"

namespace app
{
    void DrawInspector()
    {
        auto scene = APP_OBJECTS.mainScene;

        hf::ed::SetNextWindowSize(hf::vec2{ 300, 300 }, hf::ed::Condition::FirstUseEver);
        hf::ed::SetNextWindowPos(hf::vec2{ 100, 100 }, hf::ed::Condition::FirstUseEver);
        if (hf::ed::StartWindow("Inspector"))
        {
            if (hf::ed::StartDropdown("Ground"))
            {
                hf::ed::Draw("Transform", scene->ground.transform);
                hf::ed::Draw("AudioSource", scene->background_music);

                hf::ed::DrawCustomComponent("Core", &scene->ground.smoothness, [&]()
                {
                    hf::ed::DrawSlider("Smoothness", scene->ground.smoothness, 0.0f, 1.0f);
                    hf::ed::DrawColor("Color", scene->ground.color);
                });

                hf::ed::EndDropdown();
            }

            if (hf::ed::StartDropdown("Sphere"))
            {
                hf::ed::Draw("Transform", scene->sphere.transform);
                hf::ed::Draw("AudioSource", scene->background_music3D);

                hf::ed::DrawCustomComponent("Core", &scene->sphere.smoothness, [&]()
                {
                    hf::ed::DrawSlider("Smoothness", scene->sphere.smoothness, 0.0f, 1.0f);
                });

                hf::ed::EndDropdown();
            }

            hf::ed::EndWindow();
        }
    }
}