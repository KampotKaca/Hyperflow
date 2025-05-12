#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        bindingIndex = info.bindingId;
        bindings = std::vector<UniformBufferBindingInfo>(info.bindingCount);
        bufferSize = 0;

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto bindingInfo = info.pBindings[i];
            bindings[i] = bindingInfo;

            VkDescriptorSetLayoutBinding layout
            {
                .binding = info.bindingId + i,
                .descriptorType = (VkDescriptorType)bindingInfo.type,
                .descriptorCount = bindingInfo.arraySize,
                .stageFlags = (uint32_t)bindingInfo.usageFlags,
                .pImmutableSamplers = nullptr,
            };
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings[i] = layout;
            bufferSize += bindingInfo.elementSizeInBytes * bindingInfo.arraySize;
        }

        VkDescriptorSetLayoutCreateInfo uboLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = info.bindingCount,
            .pBindings = GRAPHICS_DATA.preAllocBuffers.descLayoutBindings,
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
            &uboLayoutInfo, nullptr, &layout));

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkCreateBufferInfo bufferInfo
            {
                .size = bufferSize,
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

    void SetupUniform(UniformBuffer buffer, const VkDescriptorSet* pDescriptors)
    {
        if (!IsValidUniform(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform buffer");
        auto& uniform = GRAPHICS_DATA.uniformBuffers[buffer - 1];
        memcpy(uniform.descriptorSets, pDescriptors, sizeof(VkDescriptorSet) * FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo
            {
                .buffer = uniform.buffers[i],
                .offset = 0,
                .range = uniform.bufferSize,
            };

            GRAPHICS_DATA.preAllocBuffers.descWrites[i] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = uniform.descriptorSets[i],
                .dstBinding = uniform.bindingIndex,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfo,
                .pTexelBufferView = nullptr,
            };
        }

        vkUpdateDescriptorSets(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                FRAMES_IN_FLIGHT, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }

    void UploadUniforms(const VkRenderer* rn, const UniformBufferUploadInfo& info)
    {
        auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.uploadCount; i++)
        {
            auto& uploadInfo = info.pUploads[i];
            if (!uploadInfo.data) continue;
            auto& uniform = GetUniform(uploadInfo.buffer);
            memcpy((uint8_t*)uniform.memoryMappings[currentFrame] + uploadInfo.offsetInBytes,
                uploadInfo.data, uploadInfo.sizeInBytes);
            GRAPHICS_DATA.preAllocBuffers.descriptors[i] = uniform.descriptorSets[currentFrame];
        }

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)info.bindingType, rn->currentLayout,
        info.setBindingIndex, info.uploadCount, GRAPHICS_DATA.preAllocBuffers.descriptors,
        0, nullptr);
    }
}
