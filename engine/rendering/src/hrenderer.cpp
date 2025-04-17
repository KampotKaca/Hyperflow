#include "hrenderer.h"

#include <hyperflow.h>

#include "hinternal.h"
#include "../config.h"

namespace hf
{
    Renderer::Renderer(const Ref<Window>& window)
    {
        if (inter::HF.rendererCount == 0) inter::rendering::Load(VERSION);
        inter::HF.rendererCount++;
        handle = inter::rendering::CreateInstance(window.get());
    }

    Renderer::~Renderer()
    {
        if (inter::HF.rendererCount == 1)
        {
            rendering::UnloadAllResources();
        }

        inter::rendering::DestroyInstance(handle);
        handle = nullptr;
        inter::HF.rendererCount--;
        if (inter::HF.rendererCount == 0) inter::rendering::Unload();
    }

    namespace rendering
    {
        void Draw() { Draw(inter::HF.mainWindow->renderer); }
        void Draw(const Ref<Renderer>& renderer)
        {
            inter::rendering::Draw(renderer.get());
        }

        void UnloadAllResources()
        {
            shader::DestroyAll();
        }
    }
}