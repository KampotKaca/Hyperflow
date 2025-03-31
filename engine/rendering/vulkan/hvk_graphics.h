#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct GraphicsData
    {
        int rendererCount = 0;
        VkApplicationInfo appInfo{};
    };

    struct VKRendererData
    {
        void* windowHandle = nullptr;
        VkInstance instance{};
    };

    extern GraphicsData GRAPHICS_DATA;
    extern void GraphicsLoad(const char* appVersion);
    extern void GraphicsUnload();

}

#endif //HVK_GRAPHICS_H
