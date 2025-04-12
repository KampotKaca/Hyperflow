#include "hrenderer.h"
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
        inter::rendering::DestroyInstance(handle);
        handle = nullptr;
        inter::HF.rendererCount--;
        if (inter::HF.rendererCount == 0) inter::rendering::Unload();
    }
}