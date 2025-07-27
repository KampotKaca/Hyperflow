#include "hvk_platform.h"

namespace hf
{
    std::array REQUIRED_EXTENSIONS
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_win32_surface",
#if DEBUG
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