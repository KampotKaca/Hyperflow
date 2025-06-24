#ifndef HVK_BUFFER_H
#define HVK_BUFFER_H

#include "hvk_shared.h"

namespace hf
{
    struct VkBufferBase
    {
        explicit VkBufferBase(const BufferDefinitionInfo& info, BufferMemoryType memoryType, const uint8_t* data,
            VkDescriptorType descriptorType, VkBufferUsageFlags usage);
        virtual ~VkBufferBase();

        bool isLoaded = false;
        BufferMemoryType memoryType{};
        uint32_t bindingIndex{};
        uint32_t bufferSize{};
        VkDescriptorType descriptorType{};
        VkDescriptorSetLayout layout{};
        VkBuffer buffers[FRAMES_IN_FLIGHT]{};
        VmaAllocation memoryRegions[FRAMES_IN_FLIGHT]{};
        void* memoryMappings[FRAMES_IN_FLIGHT]{};
        VkDescriptorSet descriptorSets[FRAMES_IN_FLIGHT]{};
        std::vector<BufferBindingInfo> bindings{};
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
}

#endif //HVK_BUFFER_H
