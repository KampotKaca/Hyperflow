#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings(info.bindingCount);
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto bindingInfo = info.pBindings[i];
            bindings[i] =
            {
                .binding = bindingInfo.bindingId,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = bindingInfo.arraySize,
                .stageFlags = (uint32_t)bindingInfo.usageStageFlags,
                .pImmutableSamplers = nullptr,
            };
        }

        VkDescriptorSetLayoutCreateInfo uboLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = (uint32_t)bindings.size(),
            .pBindings = bindings.data(),
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &uboLayoutInfo, nullptr, &layout));

        uint32_t allocationSize = 0;
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& binding = info.pBindings[i];
            allocationSize += binding.elementSizeInBytes * binding.arraySize;
        }

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkCreateBufferInfo bufferInfo
            {
                .size = allocationSize,
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .memoryType = BufferMemoryType::WriteOnly,
                .pQueueFamilies = nullptr,
                .familyCount = 0,
            };
            CreateBuffer(bufferInfo, &buffers[i], &memoryRegions[i]);
            vmaMapMemory(GRAPHICS_DATA.allocator, memoryRegions[i], &memoryMappings[i]);
        }
    }

    VkUniformBuffer::~VkUniformBuffer()
    {
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            vmaUnmapMemory(GRAPHICS_DATA.allocator, memoryRegions[i]);
            vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffers[i], memoryRegions[i]);
        }

        vkDestroyDescriptorSetLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, layout, nullptr);
    }

    bool IsValidUniform(UniformBuffer buffer)
    {
        return buffer > 0 && buffer <= GRAPHICS_DATA.uniformBuffers.size();
    }

    const VkUniformBuffer& GetUniform(UniformBuffer buffer)
    {
        if (!IsValidUniform(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform buffer");
        return GRAPHICS_DATA.uniformBuffers[buffer - 1];
    }

    void UploadUniform(const VKRenderer* rn, UniformBuffer buffer, const void* data, uint32_t offset, uint32_t size)
    {
        auto& uniform = GetUniform(buffer);
        memcpy(uniform.memoryMappings[rn->currentFrame], (uint8_t*)data + offset, size);
    }
}
