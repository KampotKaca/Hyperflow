#include "hvk_descriptorbuffer.h"
#include "hvk_buffer.h"
#include "hvk_graphics.h"

namespace hf
{
    constexpr static VkDeviceSize AlignUp(const VkDeviceSize value, const VkDeviceSize alignment)
    { return (value + alignment - 1) & ~(alignment - 1); }

    static void ComputeDescriptorTypes(VkDescriptorTypeCount* pTypes, uint32_t count)
    {
        const auto& props = GRAPHICS_DATA.device.descBufferProperties;
        const VkDeviceSize alignment = props.descriptorBufferOffsetAlignment;

        for (uint32_t i = 0; i < count; i++)
        {
            auto& type = pTypes[i];
            switch (type.type)
            {
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: type.typeSize = props.combinedImageSamplerDescriptorSize; break;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:          type.typeSize = props.sampledImageDescriptorSize;         break;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:          type.typeSize = props.storageImageDescriptorSize;         break;

            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:   type.typeSize = props.uniformTexelBufferDescriptorSize;   break;
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:   type.typeSize = props.storageTexelBufferDescriptorSize;   break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:         type.typeSize = props.uniformBufferDescriptorSize;        break;
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:         type.typeSize = props.storageBufferDescriptorSize;        break;
                default: throw GENERIC_EXCEPT("[Hyperflow]", "Unsupported descriptor type %i", type);
            }

            type.alignment = AlignUp(type.typeSize, alignment);
            type.bufferSize = type.alignment * type.descriptorCount;
        }
    }

    static uint64_t ComputeDescriptorBufferSize(VkDescriptorTypeCount* pTypes, uint32_t count)
    {
        ComputeDescriptorTypes(pTypes, count);
        VkDeviceSize totalSize = 0;
        for (uint32_t i = 0; i < count; i++) totalSize += pTypes[i].bufferSize;

        return totalSize;
    }

    VkDescriptorBuffer::VkDescriptorBuffer(VkDescriptorTypeCount* pTypes, uint32_t count, VkBufferUsageFlagBits typeFlags) :
        VkBufferBase(BufferMemoryType::WriteOnly, nullptr,
        VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, 0,
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | typeFlags,
        ComputeDescriptorBufferSize(pTypes, count))
    {
        VkDeviceSize size = 0;
        for (uint32_t i = 0; i < count; i++)
        {
            const auto& type = pTypes[i];
            VkDescriptorTypeData typeData{};
            typeData.offset = size;
            typeData.size = type.bufferSize;
            typeData.alignment = type.alignment;
            typeData.typeSize = type.typeSize;
            typeData.usageMasks = std::vector<uint64_t>((uint32_t)glm::ceil((float_t)type.descriptorCount / 64.0f));
            typeDatas[type.type] = typeData;
            size += type.bufferSize;
        }

        VkBufferDeviceAddressInfoEXT addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_EXT;
        addressInfo.buffer = buffers[0];

        address = GRAPHICS_DATA.extensionFunctions.vkGetBufferDeviceAddressKHR(GRAPHICS_DATA.device.logicalDevice.device, &addressInfo);
    }

    static VkResult AllocateAndGetLocation(const URef<VkDescriptorBuffer>& descBuffer, const VkDescriptorGetInfoEXT* getInfo, VkDescriptorLocation& descriptor)
    {
        auto& typeInfo = descBuffer->typeDatas[getInfo->type];
        const auto location = typeInfo.Allocate();
        if (location == -1) return VK_ERROR_OUT_OF_POOL_MEMORY;

        descriptor.type = getInfo->type;
        descriptor.positionIndex = location;
        descriptor.offset = typeInfo.offset + location * typeInfo.alignment;
        uint8_t* mapping = (uint8_t*)descBuffer->memoryMappings[0] + descriptor.offset;
        descriptor.descriptorMapping = mapping;

        GRAPHICS_DATA.extensionFunctions.vkGetDescriptorEXT(GRAPHICS_DATA.device.logicalDevice.device, getInfo, typeInfo.typeSize, mapping);
        return VK_SUCCESS;
    }

    VkResult GetBufferDescriptorFromBuffer(const URef<VkDescriptorBuffer>& descBuffer, const VkGetBufferDescriptorInfo& info, VkDescriptorLocation& descriptor)
    {
        VkDescriptorAddressInfoEXT addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_ADDRESS_INFO_EXT;
        addressInfo.address = info.address;
        addressInfo.range = info.size;

        VkDescriptorGetInfoEXT getInfo{};
        getInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
        getInfo.type = (VkDescriptorType)info.type;

        switch (info.type)
        {
        case VkDescriptorBufferType::UniformTexelBuffer:   getInfo.data.pUniformTexelBuffer   = &addressInfo; break;
        case VkDescriptorBufferType::StorageTexelBuffer:   getInfo.data.pStorageTexelBuffer   = &addressInfo; break;
        case VkDescriptorBufferType::UniformBuffer:        getInfo.data.pUniformBuffer        = &addressInfo; break;
        case VkDescriptorBufferType::StorageBuffer:        getInfo.data.pStorageBuffer        = &addressInfo; break;
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unsupported descriptor buffer type %i", info.type);
        }

        descriptor.usageFlags = VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;
        return AllocateAndGetLocation(descBuffer, &getInfo, descriptor);
    }

    VkResult GetImageDescriptorFromBuffer(const URef<VkDescriptorBuffer>& descBuffer, const VkGetImageDescriptorInfo& info, VkDescriptorLocation& descriptor)
    {
        VkDescriptorGetInfoEXT getInfo{};
        getInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
        getInfo.type = (VkDescriptorType)info.type;

        switch (info.type)
        {
        case VkDescriptorImageType::CombinedImageSampler: getInfo.data.pCombinedImageSampler = &info.imageInfo; break;
        case VkDescriptorImageType::SampledImage:         getInfo.data.pSampledImage         = &info.imageInfo; break;
        case VkDescriptorImageType::StorageImage:         getInfo.data.pStorageImage         = &info.imageInfo; break;
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unsupported descriptor image type %i", info.type);
        }

        descriptor.usageFlags = VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;
        return AllocateAndGetLocation(descBuffer, &getInfo, descriptor);
    }

    bool FreeDescriptor(const URef<VkDescriptorBuffer>& descBuffer, const VkDescriptorLocation& descriptor)
    {
        auto& typeInfo = descBuffer->typeDatas[descriptor.type];
        return typeInfo.Free((int32_t)descriptor.positionIndex);
    }
}