#ifndef HVK_BOUNDBUFFER_H
#define HVK_BOUNDBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"
#include "hvk_descriptorbuffer.h"

namespace hf
{
    struct VkBoundBuffer : public VkBufferBase
    {
        explicit VkBoundBuffer(const BufferDefinitionInfo& info,
            BufferMemoryType memoryType, const uint8_t* data,
            VkBufferUsageFlags usage, VkDescriptorType descriptorType);
        ~VkBoundBuffer() override;

        VkDescriptorSetLayout layout{};
        std::vector<BufferBindingInfo> bindings{};
        VkDescriptorLocation descriptorBindings[FRAMES_IN_FLIGHT]{};
    };

    bool IsValidBuffer(Buffer buffer);
    URef<VkBoundBuffer>& GetBuffer(Buffer buffer);
}

#endif //HVK_BOUNDBUFFER_H
