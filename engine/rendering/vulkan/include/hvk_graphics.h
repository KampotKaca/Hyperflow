#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hvk_shared.h"
#include "hvk_framebuffer.h"
#include "hvk_bufferattrib.h"

namespace hf
{
#if DEBUG
#define NUM_VK_VALIDATION_LAYERS 1
    extern const char* DEBUG_VALIDATION_LAYERS[NUM_VK_VALIDATION_LAYERS];

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
        void* platformInstance{};
        uint32_t supportedVersion;
        std::set<std::string> availableExtensionNames{};
        std::vector<GraphicsDevice> suitableDevices{};
        GraphicsDevice* defaultDevice;
        bool devicesAreLoaded = false;

        VkRenderPass renderPass{};
        VkPipelineLayout pipelineLayout{};
        std::vector<VkBufferAttrib> bufferAttribs{};

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
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};
        GraphicsSwapchainDetails details{};
        std::vector<VkFrameBuffer*> frameBuffers{};
    };

    struct CommandPool
    {
        VkCommandPool pool{};
        std::vector<VkCommandBuffer> buffers{};
    };

    struct VkFrame
    {
        VkSemaphore isImageAvailable{};
        VkSemaphore isRenderingFinished{};
        VkFence isInFlight{};
        VkCommandBuffer usedCommands[VULKAN_API_MAX_COMMANDS_PER_FRAME]{};
        uint32_t usedCommandCount = 0;
    };

    struct VkPipelineLayoutCreationInfo
    {
        uint32_t layoutCount = 0;
    };

    enum class VkRenderPassAttachmentType { Color };

    struct VkRenderPassCreationInfo
    {
        const VkRenderPassAttachmentType* pAttachments;
        uint32_t attachmentCount = 0;
    };

#if DEBUG
#define NUM_REQUIRED_EXTENSIONS 3
#else
#define NUM_REQUIRED_EXTENSIONS 2
#endif
#define NUM_DEVICE_EXTENSIONS 1

    extern GraphicsData GRAPHICS_DATA;

    void LoadVulkan(const inter::rendering::RendererLoadInfo& info);
    void UnloadVulkan();

    void DelayThreadUntilRenderingFinish();

    void CreateSwapchain(VkSurfaceKHR surface, uvec2 targetSize, GraphicsSwapChain* result);
    void DestroySwapchain(GraphicsSwapChain& gc, VkSwapchainKHR* swapchain);

    void CreateFrame(VkFrame* result);
    void DestroyFrame(VkFrame& frame);

    void CreatePipelineLayout(const VkPipelineLayoutCreationInfo& info, VkPipelineLayout* pipelineLayout);
    void DestroyPipelineLayout(VkPipelineLayout* pipelineLayout);

    void CreateRenderPass(const VkRenderPassCreationInfo& info, VkRenderPass* renderPass);
    void DestroyRenderPass(VkRenderPass* renderPass);

    void CreateCommandPool(const GraphicsDevice& device, CommandPool* result);
    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool);

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, uint32_t count);

    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, uvec2 targetExtents,
                                    GraphicsSwapchainDetails* result);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails);

    bool IsLayerSupported(const char* layer);
    bool IsExtensionSupported(const char* extension);

    void CreateSemaphore(const GraphicsDevice& device, VkSemaphore* semaphore);
    void DestroySemaphore(const GraphicsDevice& device, VkSemaphore& semaphore);
    void CreateFence(const GraphicsDevice& device, VkFence* fence, bool startSignaled);
    void DestroyFence(const GraphicsDevice& device, VkFence& fence);

    void WaitForFences(const GraphicsDevice& device, const VkFence* fences, uint32_t count, bool waitAll);

    extern const char* REQUIRED_EXTENSIONS[NUM_REQUIRED_EXTENSIONS];
    extern const char* DEVICE_EXTENSIONS[NUM_DEVICE_EXTENSIONS];
}

#endif //HVK_GRAPHICS_H
