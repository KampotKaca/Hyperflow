#ifndef HVK_DESCRIPTORBUFFER_H
#define HVK_DESCRIPTORBUFFER_H

#include "hvk_shared.h"
#include "hvk_buffer.h"

namespace hf
{
    enum class VkDescriptorImageType { CombinedImageSampler = 1, SampledImage = 2, StorageImage = 3 };
    enum class VkDescriptorBufferType { UniformTexelBuffer = 4, StorageTexelBuffer = 5, UniformBuffer = 6, StorageBuffer = 7 };

    struct VkDescriptorTypeCount
    {
        VkDescriptorType type{};
        uint32_t descriptorCount{};
        VkDeviceSize alignment{};  //not needed to set will be resolved inside the function call!
        VkDeviceSize bufferSize{}; //not needed to set will be resolved inside the function call!
        VkDeviceSize typeSize{}; //not needed to set will be resolved inside the function call!
    };

    struct VkDescriptorTypeData
    {
        VkDeviceSize offset{};
        VkDeviceSize size{};
        VkDeviceSize alignment{};
        VkDeviceSize typeSize{};
        std::vector<uint64_t> usageMasks{};

        int32_t Allocate()
        {
            for (uint32_t i = 0; i < usageMasks.size(); i++)
            {
                auto& mask = usageMasks[i];
                if (mask == ~0llu) continue;
                const auto location = utils::GetFirstBitZero64(usageMasks[i]);
                const auto index = (int32_t)(location + i * 64);
                if (index >= (size / alignment)) return -1;

                mask |= 1llu << location;
                return index;
            }
            return -1;
        }

        bool Free(int32_t location)
        {
            const uint32_t index = location / 64;
            const uint32_t offset = location % 64;
            if (index >= usageMasks.size()) return false;
            usageMasks[index] &= ~(1llu << offset);
            return true;
        }
    };

    struct VkLocatedDescriptor
    {
        VkDescriptorType type{};
        uint32_t positionIndex{};
        uint8_t* descriptorMapping{};
    };

    struct VkGetBufferDescriptorInfo
    {
        VkDescriptorBufferType type{};
        VkDeviceAddress address{};
    };

    struct VkGetImageDescriptorInfo
    {
        VkDescriptorImageType type{};
        VkDescriptorImageInfo imageInfo{};
    };

    struct VkDescriptorBuffer final : public VkBufferBase
    {
        explicit VkDescriptorBuffer(VkDescriptorTypeCount* pTypes, uint32_t count);
        ~VkDescriptorBuffer() override = default;

        VkDescriptorTypeData typeDatas[VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1];
    };

    VkResult GetBufferDescriptorFromBuffer(VkDescriptorBuffer* descBuffer, const VkGetBufferDescriptorInfo& info, VkLocatedDescriptor& descriptor);
    VkResult GetImageDescriptorFromBuffer(VkDescriptorBuffer* descBuffer, const VkGetImageDescriptorInfo& info, VkLocatedDescriptor& descriptor);
    bool FreeDescriptor(VkDescriptorBuffer* descBuffer, const VkLocatedDescriptor& descriptor);

    void BindLayout(const VkDescriptorBuffer* descBuffer, VkDescriptorSetLayout layout, const VkLocatedDescriptor& descriptor);
}

#endif //HVK_DESCRIPTORBUFFER_H
