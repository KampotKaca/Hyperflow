#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hvk_shared.h"
#include "hvk_framebuffer.h"
#include "hvk_bufferattrib.h"
#include "hvk_vertbuffer.h"
#include "hvk_uniformbuffer.h"
#include "hvk_shadersetup.h"
#include "hvk_renderpass.h"
#include "hvk_texturelayout.h"
#include "hvk_texturepack.h"
#include "hvk_texturepackallocator.h"
#include "hvk_uniformallocator.h"
#include "hvk_texturesampler.h"
#include "hvk_platform.h"

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

    struct CommandPool
    {
        VkCommandPool pool{};
        std::vector<VkCommandBuffer> buffers{};
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats{};
        std::vector<VkPresentModeKHR> presentModes{};
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily{};
        std::optional<uint32_t> presentFamily{};
        std::optional<uint32_t> transferFamily{};
        std::optional<uint32_t> computeFamily{};

        [[nodiscard]] bool IsComplete() const;
    };

    struct LogicalDevice
    {
        VkDevice device{};
        VkQueue graphicsQueue{};
        VkQueue presentQueue{};
        VkQueue transferQueue{};
        VkQueue computeQueue{};
    };

    struct DeviceTransferData
    {
        std::vector<uint32_t> indices{};
        VkSharingMode sharingMode{};
    };

    struct GraphicsDevice
    {
        VkPhysicalDevice device{};
        VkPhysicalDeviceProperties properties{};
        VkPhysicalDeviceFeatures features{};
        int32_t score{};

        QueueFamilyIndices familyIndices{};
        LogicalDevice logicalDevice{};
        DeviceTransferData transferData{};
        VkPhysicalDeviceMemoryProperties memProps{};
        VkFormatProperties formatProps[(uint32_t)TextureFormat::Count]{};
    };

    struct VkPlatform
    {
        uint32_t (*createVulkanSurfaceFunc)(void* windowHandle, void* instance, void* surfaceResult);
        void* platformDll{};
        VulkanPlatformAPI* api{};
    };

    struct VkCopyBufferToBufferOperation
    {
        VkBuffer srcBuffer{};
        VmaAllocation srcMemory{};
        VkBuffer dstBuffer{};
        VmaAllocation dstMemory{};

        VkBufferCopy pRegions[VULKAN_API_MAX_NUM_COPY_REGIONS]{};
        uint32_t regionCount = 0;

        void* uData{};
        void (*taskCompletionCallback)(void* udata){};
        bool deleteSrcAfterCopy = false;
    };

    struct VkCopyBufferToImageOperation
    {
        VkBuffer srcBuffer{};
        VmaAllocation srcMemory{};
        VkImageLayout srcLayout{};

        VkImage dstImage{};
        VmaAllocation dstMemory{};
        VkImageLayout dstLayout{};

        VkFormat format{};
        VkImageAspectFlags aspectFlags{};

        VkBufferImageCopy pRegions[VULKAN_API_MAX_NUM_COPY_REGIONS]{};
        uint32_t regionCount = 0;
        uint32_t mipLevels = 1;
        uvec2 imageSize{};

        void* uData{};
        void (*taskCompletionCallback)(void* udata){};
        bool deleteSrcAfterCopy = false;
    };

    struct ImageTransitionInfo
    {
        VkImage image{};
        VkFormat format{};
        VkImageAspectFlags aspectFlags{};
        uint32_t mipLevels = 1;
    };

    struct ImageTransitionArray
    {
        ImageTransitionInfo infos[VK_MAX_IMAGE_BARRIERS]{};
        uint32_t count = 0;
    };

    struct PreAllocatedBuffers
    {
        VkDescriptorSet descriptors[VK_MAX_UNIFORM_AND_TEXTURE_UPLOADS]{};
        VkDescriptorSetLayoutBinding descLayoutBindings[VK_MAX_UNIFORM_AND_TEXTURE_BINDINGS]{};
        VkDescriptorSetLayout descLayouts[VK_MAX_UNIFORM_AND_TEXTURE_BINDINGS * FRAMES_IN_FLIGHT]{};
        VkDescriptorBufferInfo bufferInfos[VK_MAX_UNIFORM_AND_TEXTURE_BINDINGS * FRAMES_IN_FLIGHT]{};
        VkWriteDescriptorSet descWrites[VK_MAX_UNIFORM_AND_TEXTURE_BINDINGS * FRAMES_IN_FLIGHT]{};
        VkDescriptorImageInfo descImageBindings[VK_MAX_IMAGE_BINDINGS]{};
        VkImageMemoryBarrier imageBarriers[VK_MAX_IMAGE_BARRIERS]{};
        ImageTransitionArray imageTransitions[9]{};
    };

    struct GraphicsData
    {
        int32_t rendererCount = 0;
        std::vector<VkLayerProperties> availableLayers{};
        std::vector<VkExtensionProperties> availableExtensions{};

        VkInstance instance{};
        VkPlatform platform{};

        uint32_t supportedVersion{};
        std::set<std::string> availableExtensionNames{};
        std::vector<GraphicsDevice> suitableDevices{};
        GraphicsDevice* defaultDevice{};

        bool devicesAreLoaded = false;

        CommandPool transferPool{};
        CommandPool graphicsPool{};
        VmaAllocator allocator{};

        std::vector<VkBufferAttrib> bufferAttribs{};
        std::vector<VkUniformBuffer> uniformBuffers{};
        std::vector<VkUniformAllocator> uniformAllocators{};
        std::vector<VkTextureSampler> textureSamplers{};
        std::vector<VkTextureLayout> textureLayouts{};
        std::vector<VkShaderSetup> shaderSetups{};
        std::vector<VkDrawPass> renderPasses{};

        std::vector<VkCopyBufferToBufferOperation> bufferToBufferCopyOperations{};
        std::vector<VkCopyBufferToImageOperation> bufferToImageCopyOperations{};

        PreAllocatedBuffers preAllocBuffers{};

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
        VkSurfaceFormatKHR format{};
        VkPresentModeKHR presentMode{};
        VkExtent2D extent{};
    };

    struct SwapchainImage
    {
        VkImage image{};
        VkImageView view{};
        VkFrameBuffer* frameBuffer{};

        VkSemaphore isRenderingFinished{};
        VkFence isInFlight{};
    };

    struct GraphicsSwapChain
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        GraphicsSwapchainDetails details{};
        std::vector<SwapchainImage> images{};
    };

    struct VkFrame
    {
        VkSemaphore isImageAvailable{};
        VkCommandBuffer usedCommands[VULKAN_API_MAX_COMMANDS_PER_FRAME]{};
        uint32_t usedCommandCount = 0;
    };

    struct VkCreateBufferInfo
    {
        VkDeviceSize size{};
        VkBufferUsageFlags usage{};
        VkSharingMode sharingMode{};
        BufferMemoryType memoryType{};
        uint32_t* pQueueFamilies{};
        uint32_t familyCount{};
    };

    struct VkStaticBufferInfo
    {
        uint64_t bufferSize{};
        const void* data{};
        VkBufferUsageFlags usage{};
    };

    extern GraphicsData GRAPHICS_DATA;

    void LoadVulkan(const inter::rendering::RendererLoadInfo& info);
    void UnloadVulkan();

    void LoadDevice(void* windowHandle, VkSurfaceKHR* resultSurface);
    void UnloadDevice();

    void DelayThreadUntilRenderingFinish();

    void CreateSwapchain(VkSurfaceKHR surface, uvec2 targetSize, VsyncMode vsyncMode, GraphicsSwapChain* result);
    void DestroySwapchain(GraphicsSwapChain& gc, VkSwapchainKHR* swapchain);

    void CreateCommandPool(const GraphicsDevice& device, uint32_t familyIndex, CommandPool* result);
    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool);

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, uint32_t count);

    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, VkPresentModeKHR defaultPresentMode,
                                    uvec2 targetExtents, GraphicsSwapchainDetails* result);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails* supportDetails);

    bool IsLayerSupported(const char* layer);
    bool IsExtensionSupported(const char* extension);

    void CreateSemaphore(const GraphicsDevice& device, VkSemaphore* semaphore);
    void DestroySemaphore(const GraphicsDevice& device, VkSemaphore& semaphore);
    void CreateFence(const GraphicsDevice& device, VkFence* fence, bool startSignaled);
    void DestroyFence(const GraphicsDevice& device, VkFence& fence);

    void WaitForFences(const GraphicsDevice& device, const VkFence* fences, uint32_t count, bool waitAll);
    void CreateBuffer(const VkCreateBufferInfo& info, VkBuffer* bufferResult,  VmaAllocation* memResult);
    void AllocateImage(BufferMemoryType memoryType, VkImage image, VmaAllocation* memResult);
    void CreateStagingBuffer(uint64_t bufferSize, const void* data, VkBuffer* bufferResult, VmaAllocation* memoryResult);
    void CreateStaticBuffer(const VkStaticBufferInfo& info, VkBuffer* bufferResult, VmaAllocation* memoryResult);

    uint32_t GetMemoryType(uint32_t filter, VkMemoryPropertyFlags props);
    void StageCopyOperation(const VkCopyBufferToBufferOperation& operation);
    void StageCopyOperation(const VkCopyBufferToImageOperation& operation);
    void BufferOperation(VkCommandBuffer command, VkQueue queue, void (*CopyCallback)(VkCommandBuffer command));

    void SubmitAllOperations();
    void SubmitBufferToBufferCopyOperations();
    void SubmitBufferToImageCopyOperations();

    void UploadBufferMemory(VmaAllocation memory, const void* data, uint64_t fullOffset, uint64_t fullSize);
    void UploadBufferMemory(const void* data, void* mapping, uint64_t fullOffset, uint64_t fullSize);
}

#endif //HVK_GRAPHICS_H
