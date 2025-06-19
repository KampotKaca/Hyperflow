#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const UniformBufferDefinitionInfo& info)
    {
        bindingIndex = info.bindingId;
        bindings = std::vector<UniformBufferBindingInfo>(info.bindingCount);
        memcpy(bindings.data(), info.pBindings, sizeof(UniformBufferBindingInfo) * info.bindingCount);
        bufferSize = 0;

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto bindingInfo = info.pBindings[i];

            VkDescriptorSetLayoutBinding layout
            {
                .binding = info.bindingId + i,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = bindingInfo.arraySize,
                .stageFlags = (uint32_t)bindingInfo.usageFlags,
                .pImmutableSamplers = nullptr,
            };
            GRAPHICS_DATA.preAllocBuffers.descLayoutBindings[i] = layout;

            const auto offset = bindingInfo.elementSizeInBytes * bindingInfo.arraySize;
            if (i != info.bindingCount - 1 && offset % 256 != 0)
                LOG_ERROR("Invalid uniform alignment!!! "
                          "Some devices only support alignment 256, this is not recommended. "
                          "Alignment is only necessary when uniform has more than one binding");
            bufferSize += offset;
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
            VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, memoryRegions[i], &memoryMappings[i]));
        }

        isLoaded = true;
    }

    VkUniformBuffer::~VkUniformBuffer()
    {
        if (isLoaded)
        {
            for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
            {
                vmaUnmapMemory(GRAPHICS_DATA.allocator, memoryRegions[i]);
                vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffers[i], memoryRegions[i]);
            }

            vkDestroyDescriptorSetLayout(GRAPHICS_DATA.defaultDevice->logicalDevice.device, layout, nullptr);
            isLoaded = false;
        }
    }

    bool IsValidUniform(UniformBuffer buffer)
    {
        return buffer > 0 && buffer <= GRAPHICS_DATA.uniformBuffers.size();
    }

    URef<VkUniformBuffer>& GetUniform(UniformBuffer buffer)
    {
        if (!IsValidUniform(buffer)) throw GENERIC_EXCEPT("[Hyperflow]", "Invalid uniform buffer");
        return GRAPHICS_DATA.uniformBuffers[buffer - 1];
    }

    void SetupUniform(const URef<VkUniformBuffer>& uniform)
    {
        uint32_t totalWrites = 0;
        uint32_t bufferOffset = 0;

        for (uint32_t i = 0; i < uniform->bindings.size(); i++)
        {
            auto& binding = uniform->bindings[i];
            for (uint32_t j = 0; j < binding.arraySize; j++)
            {
                for (uint32_t frame = 0; frame < FRAMES_IN_FLIGHT; frame++)
                {
                    GRAPHICS_DATA.preAllocBuffers.bufferInfos[totalWrites] =
                    {
                        .buffer = uniform->buffers[frame],
                        .offset = bufferOffset,
                        .range = binding.elementSizeInBytes * binding.arraySize,
                    };

                    GRAPHICS_DATA.preAllocBuffers.descWrites[totalWrites] =
                    {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = uniform->descriptorSets[frame],
                        .dstBinding = uniform->bindingIndex + i,
                        .dstArrayElement = j,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &GRAPHICS_DATA.preAllocBuffers.bufferInfos[totalWrites],
                        .pTexelBufferView = nullptr,
                    };

                    totalWrites++;
                }
            }

            bufferOffset += binding.elementSizeInBytes * binding.arraySize;
        }

        vkUpdateDescriptorSets(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                totalWrites, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }

    void UploadUniforms(const VkRenderer* rn, const inter::rendering::UniformBufferUploadInfo& info)
    {
        auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.uploadPacketCount; i++)
        {
            auto& packet = info.pUploadPackets[i];
            auto& uniform = GetUniform(packet.buffer);
            memcpy((uint8_t*)uniform->memoryMappings[currentFrame] + packet.offsetInBytes,
                &info.pUniformDataBuffer[packet.uniformRange.start], packet.uniformRange.size);
        }
    }

    void BindUniforms(const VkRenderer* rn, const inter::rendering::UniformBufferBindInfo& info)
    {
        auto currentFrame = rn->currentFrame;

        for (uint32_t i = 0; i < info.uniformCount; i++)
        {
            const auto& uniform = GetUniform(info.pUniforms[i]);
            GRAPHICS_DATA.preAllocBuffers.descriptors[i] = uniform->descriptorSets[currentFrame];
        }

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)info.bindingType, rn->currentLayout,
        info.setBindingIndex, info.uniformCount, GRAPHICS_DATA.preAllocBuffers.descriptors,
        0, nullptr);
    }
}
