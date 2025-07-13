#include "hvk_texturepack.h"
#include "hvk_graphics.h"
#include "hvk_texturesampler.h"
#include "hvk_renderer.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
        : layout(info.layout)
    {
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& bInfo = info.pBindings[i];
            VkTextureBinding binding
            {
                .bindingType = binding.bindingType,
                .bindingId = bInfo.bindingIndex,
                .sampler = bInfo.sampler,
                .views = {}
            };

            for (uint32_t j = 0; j < bInfo.arraySize; j++)
            {
                auto& tInfo = bInfo.textures[j];

                switch (tInfo.type)
                {
                    case TexturePackBindingType::Texture2D:
                    case TexturePackBindingType::Cubemap:
                    {
                        auto* tex = (VkTexture*)tInfo.texture;
                        binding.views.push_back(tex->view);
                    }
                    break;
                    break;
                    case TexturePackBindingType::RenderTexture:
                    {
                        auto* tex = (VkRenderTexture*)tInfo.texture;
                        if (tInfo.index >= tex->colorAttachmentCount)
                        {
                            if (!tex->depthTexture) throw GENERIC_EXCEPT("[Hyperflow]", "Render texture invalid attachment index!");
                            binding.views.push_back(tex->depthTexture->view);
                        }
                        else binding.views.push_back(tex->colorTextures[tInfo.index]->view);
                    }
                    break;
                }
            }
            bindings[bInfo.bindingIndex] = binding;
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        bindings.clear();
    }

    void UpdateTexturePack(VkTexturePack* pack, const uint32_t* bindingIndices, uint32_t bindingCount)
    {
        uint32_t writeCount = 0;
        for (uint32_t i = 0; i < bindingCount; i++)
        {
            auto& binding = pack->bindings[bindingIndices[i]];
            auto& texSampler = GetSampler(binding.sampler);

            for (uint32_t j = 0; j < binding.views.size(); j++)
            {
                for (auto descriptor : pack->descriptors)
                {
                    GRAPHICS_DATA.preAllocBuffers.descImageBindings[writeCount] =
                    {
                        .sampler = texSampler->sampler,
                        .imageView = binding.views.atC(j),
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    };

                    GRAPHICS_DATA.preAllocBuffers.descWrites[writeCount] =
                    {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = descriptor,
                        .dstBinding = binding.bindingId,
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
                for (uint32_t j = current.texInfo->offset; j < current.texInfo->count; j++)
                {
                    auto texInfo = current.texInfo->pTextures[j];

                    switch (binding.bindingType)
                    {
                        case TexturePackBindingType::Texture2D:
                        case TexturePackBindingType::Cubemap:
                        {
                            auto* tex = (VkTexture*)texInfo.texture;
                            binding.views.at(j) = tex->view;
                        }
                        break;
                        case TexturePackBindingType::RenderTexture:
                        {
                            auto* tex = (VkRenderTexture*)texInfo.texture;
                            if (texInfo.index >= tex->colorAttachmentCount)
                            {
                                if (!tex->depthTexture) throw GENERIC_EXCEPT("[Hyperflow]", "Render texture invalid attachment index!");
                                binding.views.at(j) = tex->depthTexture->view;
                            }
                            else binding.views.at(j) = tex->colorTextures[texInfo.index]->view;
                        }
                        break;
                    }
                }

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

    void BindTexturePack(const VkRenderer* rn, const VkTexturePack* pack, uint32_t setBindingIndex,
                         RenderBindingType bindingType)
    {
        const auto currentFrame = rn->currentFrame;

        vkCmdBindDescriptorSets(rn->currentCommand, (VkPipelineBindPoint)bindingType, rn->currentLayout,
        setBindingIndex, 1, &pack->descriptors[currentFrame],
        0, nullptr);
    }
}
