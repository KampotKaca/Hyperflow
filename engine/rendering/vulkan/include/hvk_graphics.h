#ifndef HVK_GRAPHICS_H
#define HVK_GRAPHICS_H

#include "hvk_shared.h"
#include "hvk_vertexbufferattribute.h"
#include "hvk_vertexbuffer.h"
#include "hvk_uniformbuffer.h"
#include "hvk_shaderlayout.h"
#include "hvk_texturelayout.h"
#include "hvk_texturepack.h"
#include "hvk_texturesampler.h"
#include "hvk_platform.h"

namespace hf
{
#if defined(VK_ENABLE_VALIDATION)
#define NUM_VK_VALIDATION_LAYERS 1
    extern const char* DEBUG_VALIDATION_LAYERS[NUM_VK_VALIDATION_LAYERS];

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
#endif

    constexpr VkDynamicState SHADER_DYNAMIC_STATES[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    constexpr VkPipelineDynamicStateCreateInfo SHADER_DYNAMIC =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = 2,
        .pDynamicStates = SHADER_DYNAMIC_STATES
    };

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
        VkPhysicalDeviceDescriptorBufferPropertiesEXT descBufferProperties{};
        VkPhysicalDeviceProperties2 properties{};
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
        inter::rendering::RendererInternalFunctions_i functions{};
        void* platformDll{};
        VulkanPlatformAPI* api{};
        VkAllocationCallbacks allocator{};
    };

    struct VkExtensionFunctions
    {
        PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{};
        PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR{};
        PFN_vkCmdPipelineBarrier2KHR vkCmdPipelineBarrier2KHR{};
        PFN_vkCmdBindDescriptorBuffersEXT vkCmdBindDescriptorBuffersEXT{};
        PFN_vkCmdSetDescriptorBufferOffsetsEXT vkCmdSetDescriptorBufferOffsetsEXT{};
        PFN_vkGetDescriptorEXT vkGetDescriptorEXT{};
        PFN_vkGetDescriptorSetLayoutSizeEXT vkGetDescriptorSetLayoutSizeEXT{};
        PFN_vkGetBufferDeviceAddressKHR vkGetBufferDeviceAddressKHR{};
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

    struct ImageTransitionInfo
    {
        VkImage image{};
        VkFormat format{};
        VkImageAspectFlags aspectFlags{};
        uint32_t mipLevels = 1;
        uint32_t layerCount = 1;
    };

    struct VkCopyBufferToImageOperation
    {
        VkBuffer srcBuffer{};
        VmaAllocation srcMemory{};
        VkImageLayout srcLayout{};

        VmaAllocation dstMemory{};
        VkImageLayout dstLayout{};

        ImageTransitionInfo imageInfo{};
        VkBufferImageCopy pRegions[VULKAN_API_MAX_NUM_COPY_REGIONS]{};
        uint32_t regionCount = 0;
        uvec2 imageSize{};

        void* uData{};
        void (*taskCompletionCallback)(void* udata){};
        bool deleteSrcAfterCopy = false;
    };

    struct PreAllocatedBuffers
    {
        std::vector<VkDescriptorSetLayout> descLayouts{};
        std::vector<VkDescriptorSetLayoutBinding> descLayoutBindings{};
        std::vector<VkDescriptorBufferBindingInfoEXT> descBindingInfos{};
        std::vector<VkImageMemoryBarrier2> imageBarriers{};
        std::vector<uint32_t> indices{};
        std::vector<VkDeviceSize> sizes{};
        std::vector<ImageTransitionInfo> imageTransitions[9]{};
    };

    struct RenderApiEditorInfo
    {
        uint32_t version{};
        VkInstance instance{};
        VkPhysicalDevice physicalDevice{};
        VkDevice device{};
        uint32_t queueFamily{};
        VkQueue queue{};
        VkDescriptorPool descriptorPool{};
        void (*CheckVkResultFn)(VkResult err){};
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
        GraphicsDevice device{};

        bool deviceIsLoaded = false;

        CommandPool transferPool{};
        CommandPool graphicsPool{};
        VmaAllocator allocator{};
        URef<VkDescriptorBuffer> bufferDescriptorBuffer{};
        URef<VkDescriptorBuffer> imageDescriptorBuffer{};

        std::vector<URef<VkVertexBufferAttribute>> bufferAttribs{};
        std::vector<URef<VkBoundBuffer>> boundBuffers{};
        std::vector<URef<VkTextureSampler>> textureSamplers{};
        std::vector<URef<VkTextureLayout>> textureLayouts{};
        std::vector<URef<VkShaderLayout>> shaderLayouts{};

        std::vector<VkCopyBufferToBufferOperation> bufferToBufferCopyOperations{};
        std::vector<VkCopyBufferToImageOperation> bufferToImageCopyOperations{};

        PreAllocatedBuffers preAllocBuffers{};
        RenderApiEditorInfo* editorInfo{};
        VkExtensionFunctions extensionFunctions{};

#if defined(VK_ENABLE_VALIDATION)
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
        VkMemoryPropertyFlags requiredFlags{};
        VmaAllocationCreateFlags allocationFlags{};
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

    void LoadVulkan(const inter::rendering::RendererLoadInfo_i& info);
    void UnloadVulkan();

    void* LoadEditorInfo();
    void UnloadEditorInfo();

    void LoadDevice(void* windowHandle, VkSurfaceKHR* resultSurface);
    void UnloadDevice();

    void WaitForDevice();

    void CreateSwapchain(VkSurfaceKHR surface, uvec2 targetSize, VsyncMode vsyncMode, GraphicsSwapChain& result);
    void DestroySwapchain(const GraphicsSwapChain& gc, VkSwapchainKHR* swapchain);

    void CreateCommandPool(const GraphicsDevice& device, uint32_t familyIndex, CommandPool* result);
    void DestroyCommandPool(const GraphicsDevice& device, CommandPool& pool);

    void CreateCommandBuffers(const GraphicsDevice& device, CommandPool* pool, uint32_t count);

    bool GetAvailableSurfaceDetails(const SwapChainSupportDetails& swapChainSupportDetails,
                                    VkFormat targetFormat, VkPresentModeKHR targetPresentMode, VkPresentModeKHR defaultPresentMode,
                                    uvec2 targetExtents, GraphicsSwapchainDetails& result);

    bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device);
    void QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, SwapChainSupportDetails& supportDetails);

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
