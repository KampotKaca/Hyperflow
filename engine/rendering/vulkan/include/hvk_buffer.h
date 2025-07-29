#ifndef HVK_BUFFER_H
#define HVK_BUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkBufferBase
    {
        explicit VkBufferBase(BufferMemoryType memoryType, const uint8_t* data,
        VkMemoryPropertyFlags requiredFlags, VkBufferUsageFlags usage, uint64_t bufferSize);
        virtual ~VkBufferBase();

        bool isLoaded = false;
        BufferMemoryType memoryType{};
        uint64_t bufferSize{};
        VkBuffer buffers[FRAMES_IN_FLIGHT]{};
        VmaAllocation memoryRegions[FRAMES_IN_FLIGHT]{};
        void* memoryMappings[FRAMES_IN_FLIGHT]{};
    };

    bool IsValidBuffer(Buffer buffer);
    URef<VkBufferBase>& GetBuffer(Buffer buffer);
    void SetupBuffer(const URef<VkBufferBase>& buffer);

    inline uint32_t GetBufferCount(BufferMemoryType memType)
    {
        switch (memType)
        {
            case BufferMemoryType::Static: case BufferMemoryType::WriteOnly: return 1;
            case BufferMemoryType::PerFrameWriteOnly: return FRAMES_IN_FLIGHT;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unimplemented buffer memory type");
        }
    }

    constexpr uint64_t ComputeBufferSize(const BufferDefinitionInfo& info)
    {
        uint64_t size = 0;
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            const auto bindingInfo = info.pBindings[i];
            const auto offset = bindingInfo.elementSizeInBytes * bindingInfo.arraySize;
            if (i != info.bindingCount - 1 && offset % 256 != 0)
                LOG_ERROR("Invalid buffer alignment!!! "
                          "Some devices only support alignment 256, this is not recommended. "
                          "Alignment is only necessary when uniform has more than one binding");
            size += offset;
        }
        return size;
    }
}

#endif //HVK_BUFFER_H
