#include "hvk_texturepack.h"
#include "hvk_graphics.h"
#include "hvk_texturesampler.h"
#include "hvk_renderer.h"
#include "hyperflow.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
    {
        setBindingIndex = info.setBindingIndex;
        bindings = std::vector<VkTextureBinding>(info.bindingCount);
        for (auto &descriptors : descriptorCache)
            descriptors = std::vector<VkDescriptorSet>(info.bindingCount);

        layout = info.layout;
        bindingType = info.bindingType;

        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& bInfo = info.pBindings[i];
            auto& binding = bindings[i];
            binding =
            {
                .bindingId = bInfo.bindingId,
                .sampler = bInfo.sampler,
            };

            binding.textures = std::vector<VkTexture*>(bInfo.textures.size());
            memcpy(binding.textures.data(), bInfo.textures.data(), bInfo.textures.size() * sizeof(VkTexture*));
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        for (auto & descriptors : descriptorCache) descriptors.clear();

        for (auto& binding : bindings)
       {
            binding.textures.clear();
            for (auto& desc : binding.descriptors) desc = VK_NULL_HANDLE;
        }
    }

    void UpdateTextureBinding(const VkTexturePack* pack, uint32_t bindingIndex,
        uint32_t offset, uint32_t size)
    {
        auto& binding = pack->bindings[bindingIndex];
        auto& texSampler = GetSampler(binding.sampler);

        for (size_t i = 0; i < size; i++)
        {
            GRAPHICS_DATA.preAllocBuffers.descImageBindings[i] =
            {
                .sampler = texSampler.sampler,
                .imageView = binding.textures[offset + i]->view,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };
        }

        for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            GRAPHICS_DATA.preAllocBuffers.descWrites[i] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = binding.descriptors[i],
                .dstBinding = binding.bindingId,
                .dstArrayElement = offset,
                .descriptorCount = size,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = GRAPHICS_DATA.preAllocBuffers.descImageBindings,
            };
        }

        vkUpdateDescriptorSets(GRAPHICS_DATA.defaultDevice->logicalDevice.device,
                FRAMES_IN_FLIGHT, GRAPHICS_DATA.preAllocBuffers.descWrites,
                0, nullptr);
    }

    void SetTextureBinding(VkTexturePack* pack, uint32_t bindingIndex, TextureSampler sampler,
        VkTexture** pTextures, uint32_t offset, uint32_t size)
    {
        DelayThreadUntilRenderingFinish();
        auto& binding = pack->bindings[bindingIndex];
        if (size > 0) memcpy(&binding.textures[offset], pTextures, size * sizeof(VkTexture*));

        if (binding.sampler != sampler)
        {
            binding.sampler = sampler;
            UpdateTextureBinding(pack, bindingIndex, 0, binding.textures.size());
        }
        else if (size > 0) UpdateTextureBinding(pack, bindingIndex, offset, size);
        else LOG_WARN("Unnecessary set binding call, noting changed");
    }

    void BindTexturePack(VkRenderer* rn, VkTexturePack* pack)
    {
        auto currentFrame = rn->currentFrame;
        auto& descriptors = pack->descriptorCache[currentFrame];

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)pack->bindingType, rn->currentLayout,
        pack->setBindingIndex, descriptors.size(), descriptors.data(),
        0, nullptr);
    }
}
