#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

namespace hf
{
    struct GraphicsData
    {
        int32_t rendererCount = 0;
        std::vector<VkLayerProperties> availableLayers{};
        std::vector<VkExtensionProperties> availableExtensions{};
        std::set<std::string> availableExtensionNames{};
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
    extern bool GraphicsValidateLayerSupport(const char* layer);
    extern bool GraphicsValidateExtensionSupport(const char* extension);

    extern const std::vector<const char*> REQUIRED_EXTENSIONS;
}

#endif //HVK_GRAPHICS_H
