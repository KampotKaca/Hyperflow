#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hshared.h"
#include <vulkan/vulkan.h>

#include "hvk_framebuffer.h"
#include "../config.h"

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

    struct VKRendererData
    {
        void* windowHandle = nullptr;
        GraphicsSwapChain swapchain{};
        VkViewport viewport{};
        VkRect2D scissor{};
        CommandPool commandPool{};
        uvec2 targetSize{};
        SwapChainSupportDetails swapchainSupport{};

        VkCommandBuffer currentCommand{};
        VkRenderPass currentPass{};

        std::vector<VkFrame> frames{};
        uint32_t currentFrame = 0;
        uint32_t imageIndex{};
        bool frameBufferResized = false;
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

    void CreateVulkanRenderer(VKRendererData* rn);
    void DestroyVulkanRenderer(VKRendererData* rn);

    void CreateSurface(VKRendererData* rn);
    void DestroySurface(VKRendererData* rn);

    void SetupViewportAndScissor(VKRendererData* rn);
    void CreateSwapchain(VkSurfaceKHR surface, const SwapChainSupportDetails& scs, uvec2 targetSize, GraphicsSwapChain* result);
    void DestroySwapchain(GraphicsSwapChain& swapchain);
    void PresentSwapchain(VKRendererData* rn);
    bool AcquireNextImage(VKRendererData* rn);

    void CreateRenderPass(VkRenderPass* renderPass);
    void DestroyRenderPass(const VkRenderPass& renderPass);

    void BeginRenderPass(const VkRenderPass& renderPass, VKRendererData* rn);
    void EndRenderPass(VKRendererData* rn);

    void CreateCommandPool(const GraphicsDevice& device, CommandPool* result);
    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool);

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, uint32_t count);

    void BeginCommandBuffer(VKRendererData* rn, VkCommandBuffer buffer);
    void EndCommandBuffer(VKRendererData* rn);
    void SubmitCommands(VKRendererData* rn);

    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, uvec2 targetExtents,
                                    GraphicsSwapchainDetails* result);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
    void CreateRendererFrameBuffers(VKRendererData* rn);
    void DestroyRendererFrameBuffers(VKRendererData* rn);
    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails);

    bool IsLayerSupported(const char* layer);
    bool IsExtensionSupported(const char* extension);

    void UploadViewportAndScissor(const VKRendererData* rn);

    void CreateSemaphore(const GraphicsDevice& device, VkSemaphore* semaphore);
    void DestroySemaphore(const GraphicsDevice& device, VkSemaphore& semaphore);
    void CreateFence(const GraphicsDevice& device, VkFence* fence, bool startSignaled);
    void DestroyFence(const GraphicsDevice& device, VkFence& fence);

    void WaitForFences(const GraphicsDevice& device, const VkFence* fences, uint32_t count, bool waitAll);

    extern const std::vector<const char*> REQUIRED_EXTENSIONS;
    extern const std::vector<const char*> DEVICE_EXTENSIONS;
}

#endif //HVK_GRAPHICS_H
