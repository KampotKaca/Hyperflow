#include "hshared.h"
#include <vulkan/vulkan.h>

VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo,
                          const VkAllocationCallbacks* pAllocator,
                          VkInstance* instance)
{

    if (pCreateInfo == nullptr || instance == nullptr)
    {
        LOG_ERROR("Null pointer passed to required parameter!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return real_vkCreateInstance(pCreateInfo, pAllocator, instance);
}