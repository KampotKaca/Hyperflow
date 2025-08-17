#include "hvk_platform.h"

namespace hf
{
    std::array REQUIRED_EXTENSIONS
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_wayland_surface",
#if defined(VK_ENABLE_VALIDATION)
        "VK_EXT_debug_utils"
#endif
    };

    VulkanPlatformAPI* GetAPI()
    {
        static VulkanPlatformAPI api =
        {
            .requiredExtensionCount = REQUIRED_EXTENSIONS.size(),
        };

        for (uint32_t i = 0; i < REQUIRED_EXTENSIONS.size(); i++) api.requiredExtension[i] = REQUIRED_EXTENSIONS[i];

        return &api;
    }
}