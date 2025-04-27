#ifndef HVK_VERTBUFFER_H
#define HVK_VERTBUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkVertBuffer
    {
        VkVertBuffer(const VertBufferCreationInfo& info);
        ~VkVertBuffer();

        VkBuffer buffer{};
        VkDeviceMemory bufferMemory{};
        BufferAttrib attrib{};
        uint32_t vertCount = 0;
        VertBufferMemoryType memoryType = VertBufferMemoryType::Static;
    };

    struct VkCopyBufferOperation
    {
        VkBuffer srcBuffer{};
        VkDeviceMemory srcMemory{};
        VkBuffer dstBuffer{};
        VkDeviceMemory dstMemory{};

        VkBufferCopy pRegions[VULKAN_API_MAX_NUM_COPY_REGIONS];
        uint32_t regionCount = 0;
        bool deleteSrcAfterCopy = false;
    };

    void StageCopyOperation(const VkCopyBufferOperation& operation);
    void SubmitStagedCopyOperations();

    void UploadBufferMemory(VkDeviceMemory memory, const void* data, uint64_t fullOffset, uint64_t fullSize);
    void CopyBufferContents(const VkCopyBufferOperation* pOperations, uint32_t operationCount);

}

#endif //HVK_VERTBUFFER_H
