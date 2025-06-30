#include "hvk_texturepack.h"
#include "hvk_graphics.h"
#include "hvk_texturesampler.h"
#include "hvk_renderer.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
        : bindingType(info.bindingType), layout(info.layout)
    {
        bindings = std::vector<VkTextureBinding>(info.bindingCount);
        bindingType = info.bindingType;

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& bInfo = info.pBindings[i];
            auto& binding = bindings[i];
            binding =
            {
                .sampler = bInfo.sampler,
            };

            binding.textures = std::vector<VkTexture*>(bInfo.count);
            memcpy(binding.textures.data(), bInfo.pTextures, bInfo.count * sizeof(VkTexture*));
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        bindings.clear();
    }

    void UpdateTexturePack(const VkTexturePack* pack, const uint32_t* bindingIndices, uint32_t bindingCount)
    {
        uint32_t writeCount = 0;
        for (uint32_t i = 0; i < bindingCount; i++)
        {
            auto& binding = pack->bindings[bindingIndices[i]];
            auto& texSampler = GetSampler(binding.sampler);

            for (uint32_t j = 0; j < binding.textures.size(); j++)
            {
                for (auto descriptor : pack->descriptors)
                {
                    GRAPHICS_DATA.preAllocBuffers.descImageBindings[writeCount] =
                    {
                        .sampler = texSampler->sampler,
                        .imageView = binding.textures[j]->view,
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    };

                    GRAPHICS_DATA.preAllocBuffers.descWrites[writeCount] =
                    {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = descriptor,
                        .dstBinding = pack->bindingId + i,
                        .dstArrayElement = j,
                        .descriptorCount = 1,
                        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .pImageInfo = &GRAPHICS_DATA.preAllocBuffers.descImageBindings[writeCount],
                    };

                    writeCount++;
                }
            }
        }

        vkUpdateDescriptorSets(GRAPHICS_DATA.device.logicalDevice.device,
                writeCount, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }

    void SetTextureBinding(VkTexturePack* pack, const inter::rendering::TexturePackBindingUploadInfo* bindings, uint32_t bindingCount)
    {
        uint32_t count = 0;
        for (uint32_t i = 0; i < bindingCount; i++)
        {
            bool wasModified = false;
            auto& current = bindings[i];
            auto& binding = pack->bindings[current.bindingIndex];
            if (current.sampler.has_value())
            {
                binding.sampler = current.sampler.value();
                wasModified = true;
            }

            if (current.texInfo.has_value() && current.texInfo->count > 0)
            {
                memcpy(&binding.textures[current.texInfo->offset], current.texInfo->pTextures, current.texInfo->count * sizeof(VkTexture*));
                wasModified = true;
            }

            if (wasModified)
            {
                GRAPHICS_DATA.preAllocBuffers.indices[count] = current.bindingIndex;
                count++;
            }
        }

        if (count > 0)
        {
            UpdateTexturePack(pack, GRAPHICS_DATA.preAllocBuffers.indices, count);
        }
        else LOG_WARN("Unnecessary set binding call, noting changed");
    }

    void BindTexturePack(const VkRenderer* rn, const VkTexturePack* pack, uint32_t setBindingIndex)
    {
        const auto currentFrame = rn->currentFrame;

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)pack->bindingType, rn->currentLayout,
        setBindingIndex, 1, &pack->descriptors[currentFrame],
        0, nullptr);
    }
}
