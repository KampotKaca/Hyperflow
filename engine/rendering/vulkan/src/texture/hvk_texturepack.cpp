#include "hvk_texturepack.h"
#include "hvk_graphics.h"
#include "hvk_texturesampler.h"
#include "hvk_renderer.h"
#include "hyperflow.h"

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

            binding.textures = std::vector<VkTexture*>(bInfo.textures.size());
            memcpy(binding.textures.data(), bInfo.textures.data(), bInfo.textures.size() * sizeof(VkTexture*));
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        bindings.clear();
    }

    void UpdateTexturePack(const VkTexturePack* pack, uint32_t bindingOffset, uint32_t bindingCount)
    {
        uint32_t writeCount = 0;
        for (uint32_t i = bindingOffset; i < bindingCount; i++)
        {
            auto& binding = pack->bindings[i];
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

        vkUpdateDescriptorSets(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                writeCount, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }

    void SetTextureBinding(VkTexturePack* pack, uint32_t bindingIndex, TextureSampler sampler,
        VkTexture** pTextures, uint32_t offset, uint32_t size)
    {
        auto& binding = pack->bindings[bindingIndex];
        if (size > 0) memcpy(&binding.textures[offset], pTextures, size * sizeof(VkTexture*));

        if (binding.sampler != sampler)
        {
            binding.sampler = sampler;
            UpdateTexturePack(pack, bindingIndex, 1);
        }
        else if (size > 0) UpdateTexturePack(pack, bindingIndex, 1);
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
