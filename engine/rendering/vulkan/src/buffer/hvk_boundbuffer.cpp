#include "hvk_renderer.h"
#include "hvk_boundbuffer.h"
#include "hvk_graphics.h"

namespace hf
{
    VkBoundBuffer::VkBoundBuffer(const BufferDefinitionInfo& info,
        const BufferMemoryType memoryType, const uint8_t* data,
        const VkBufferUsageFlags usage, const VkDescriptorType descriptorType) :
        VkBufferBase(memoryType, data, 0, 0,
        usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, ComputeBufferSize(info))
    {
        bindings = List<BufferBindingInfo>(info.bindingCount);
        memcpy(bindings.data(), info.pBindings, sizeof(BufferBindingInfo) * info.bindingCount);

        GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.resize(info.bindingCount);
        GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.clear();

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto bindingInfo = info.pBindings[i];

            VkDescriptorSetLayoutBinding layout{};
            layout.binding = info.bindingId + i;
            layout.descriptorType = descriptorType;
            layout.descriptorCount = bindingInfo.arraySize;
            layout.stageFlags = (uint32_t)bindingInfo.usageFlags;
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.push_back(layout);

            const auto offset = bindingInfo.elementSizeInBytes * bindingInfo.arraySize;
            if (i != info.bindingCount - 1 && offset % 256 != 0)
                log_error("Invalid buffer alignment!!! "
                          "Some devices only support alignment 256, this is not recommended. "
                          "Alignment is only necessary when uniform has more than one binding");
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.size();
        layoutInfo.pBindings = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings.data();
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device,
            &layoutInfo, &GRAPHICS_DATA.platform.allocator, &layout));

        VkBufferDeviceAddressInfoEXT addressInfo{};
        addressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_EXT;

        VkGetBufferDescriptorInfo descInfo{};
        descInfo.type = (VkDescriptorBufferType)descriptorType;
        descInfo.size = bufferSize;

        switch (memoryType)
        {
        case BufferMemoryType::Static: case BufferMemoryType::WriteOnly:
        {
            addressInfo.buffer = buffers[0];
            descInfo.address = GRAPHICS_DATA.extensionFunctions.vkGetBufferDeviceAddressKHR(GRAPHICS_DATA.device.logicalDevice.device, &addressInfo);

            GetBufferDescriptorFromBuffer(GRAPHICS_DATA.bufferDescriptorBuffer, descInfo, descriptorBindings[0]);
            for (uint32_t i = 1; i < FRAMES_IN_FLIGHT; i++) descriptorBindings[i] = descriptorBindings[0];
        }break;
        case BufferMemoryType::PerFrameWriteOnly:
        {
            for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                addressInfo.buffer = buffers[i];
                descInfo.address = GRAPHICS_DATA.extensionFunctions.vkGetBufferDeviceAddressKHR(GRAPHICS_DATA.device.logicalDevice.device, &addressInfo);
                GetBufferDescriptorFromBuffer(GRAPHICS_DATA.bufferDescriptorBuffer, descInfo, descriptorBindings[i]);
            }

        }break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkBoundBuffer::~VkBoundBuffer()
    {
        if (isLoaded)
        {
            vkDestroyDescriptorSetLayout(GRAPHICS_DATA.device.logicalDevice.device, layout, &GRAPHICS_DATA.platform.allocator);
            for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) FreeDescriptor(GRAPHICS_DATA.bufferDescriptorBuffer, descriptorBindings[i]);
        }
    }

    void BindBuffer(const VkRenderer* rn, const ir::rdr::BindResourceInfo_i<Buffer>& info)
    {
        const auto currentFrame = rn->currentFrame;

        GRAPHICS_DATA.preAllocBuffers.descBindingInfos.resize(info.objectCount);
        GRAPHICS_DATA.preAllocBuffers.indices.resize(info.objectCount);
        GRAPHICS_DATA.preAllocBuffers.sizes.resize(info.objectCount);

        GRAPHICS_DATA.preAllocBuffers.descBindingInfos.clear();
        GRAPHICS_DATA.preAllocBuffers.indices.clear();
        GRAPHICS_DATA.preAllocBuffers.sizes.clear();

        for (uint32_t i = 0; i < info.objectCount; i++)
        {
            const auto& binding = GetBuffer(info.objects[i])->descriptorBindings[currentFrame];
            VkDescriptorBufferBindingInfoEXT bindingInfo{};
            bindingInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
            bindingInfo.address = GRAPHICS_DATA.bufferDescriptorBuffer->address;
            bindingInfo.usage = binding.usageFlags;

            GRAPHICS_DATA.preAllocBuffers.descBindingInfos.push_back(bindingInfo);
            GRAPHICS_DATA.preAllocBuffers.indices.push_back(0);
            GRAPHICS_DATA.preAllocBuffers.sizes.push_back(binding.offset); //will be used as offsets.
        }

        FinishObjectBinding(rn, (VkPipelineBindPoint)info.bindingType, info.setBindingIndex);
    }

    void FinishObjectBinding(const VkRenderer* rn, VkPipelineBindPoint bindPoint, uint32_t setBindingIndex)
    {
        const auto objectCount = (uint32_t)GRAPHICS_DATA.preAllocBuffers.descBindingInfos.size();
        GRAPHICS_DATA.extensionFunctions.vkCmdBindDescriptorBuffersEXT
        (rn->currentCommand, objectCount, GRAPHICS_DATA.preAllocBuffers.descBindingInfos.data());

        GRAPHICS_DATA.extensionFunctions.vkCmdSetDescriptorBufferOffsetsEXT
        (rn->currentCommand, bindPoint, rn->currentLayout,
            setBindingIndex, objectCount,
            GRAPHICS_DATA.preAllocBuffers.indices.data(), GRAPHICS_DATA.preAllocBuffers.sizes.data());
    }

    bool IsValidBuffer(const Buffer buffer)
    {
        return buffer > 0 && buffer <= GRAPHICS_DATA.boundBuffers.size();
    }

    URef<VkBoundBuffer>& GetBuffer(const Buffer buffer)
    {
        if (!IsValidBuffer(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid buffer");
        return GRAPHICS_DATA.boundBuffers[buffer - 1];
    }
}