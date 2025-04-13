#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

#include "hvk_framebuffer.h"

namespace hf::inter::rendering
{
#if DEBUG
    extern const std::vector<const char*> DEBUG_VALIDATION_LAYERS;

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
#endif

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool IsComplete() const;
    };

    struct LogicalDevice
    {
        VkDevice device{};
        VkQueue graphicsQueue{};
        VkQueue presentQueue{};
    };

    struct GraphicsDevice
    {
        VkPhysicalDevice device{};
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        int32_t score{};

        QueueFamilyIndices familyIndices{};
        LogicalDevice logicalDevice{};
    };

    struct GraphicsData
    {
        int32_t rendererCount = 0;
        std::vector<VkLayerProperties> availableLayers{};
        std::vector<VkExtensionProperties> availableExtensions{};

        VkInstance instance{};
        uint32_t supportedVersion;
        std::set<std::string> availableExtensionNames{};
        std::vector<GraphicsDevice> suitableDevices{};
        GraphicsDevice* defaultDevice;
        bool devicesAreLoaded = false;

        VkRenderPass renderPass{};

#if DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = DebugCallback
        };
        VkDebugUtilsMessengerEXT debugMessenger{};
#endif
    };

    struct GraphicsSwapchainDetails
    {
        VkSurfaceFormatKHR format;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;
    };

    struct GraphicsSwapChain
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};
        GraphicsSwapchainDetails details{};
        std::vector<VkFrameBuffer*> frameBuffers{};
    };

    struct VKRendererData
    {
        void* windowHandle = nullptr;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        GraphicsSwapChain swapchain{};
        VkViewport viewport{};
        VkRect2D scissor{};
    };

    enum class PipelineBlendType { None, Alpha, Logical };
    struct VkPipelineInfo
    {
        VkPipelineShaderStageCreateInfo* pStages = nullptr;
        uint32_t stageCount = 0;
        PipelineBlendType blendingMode = PipelineBlendType::None;
        VkLogicOp blendingOp = VK_LOGIC_OP_XOR; //Setting will be used only if you use Logical Blending
        VkPipelineLayout layout{};
        VkRenderPass renderPass{};
    };

    extern GraphicsData GRAPHICS_DATA;

    bool IsLayerSupported(const char* layer);
    bool IsExtensionSupported(const char* extension);

    void CreatePlatformSurface(VKRendererData* rendererData);
    void DestroyPlatformSurface(VKRendererData* rendererData);

    void CreateSurface(VKRendererData* rendererData);
    void DestroySurface(VKRendererData* rendererData);

    void CreateRenderPass(VkRenderPass* renderPass);
    void DestroyRenderPass(const VkRenderPass& renderPass);

    extern const std::vector<const char*> REQUIRED_EXTENSIONS;
    extern const std::vector<const char*> DEVICE_EXTENSIONS;
}

#endif //HVK_GRAPHICS_H
