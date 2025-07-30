#include "hvk_texturepack.h"
#include "hvk_graphics.h"
#include "hvk_texturesampler.h"
#include "hvk_renderer.h"

namespace hf
{
    static void CreateDescriptorForBinding(VkTextureBinding& binding, uint32_t from, uint32_t size);

    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo_i& info)
        : layout(info.layout)
    {
        for (uint32_t i = 0; i < info.bindingCount; i++)
        {
            auto& bInfo = info.pBindings[i];
            VkTextureBinding binding
            {
                .bindingType = bInfo.type,
                .bindingId = bInfo.bindingIndex,
                .sampler = bInfo.sampler,
                .bindingArray = {}
            };

            switch (bInfo.type)
            {
                case TexturePackBindingType::Texture2D:
                case TexturePackBindingType::Cubemap:
                {
                    for (uint32_t j = 0; j < bInfo.arraySize; j++)
                    {
                        auto& tInfo = bInfo.textures[j];

                        if (tInfo.texture)
                        {
                            const auto* tex = (VkTexture*)tInfo.texture;
                            auto newBinding = VkTextureBindingViewData{};
                            newBinding.view = tex->view;
                            newBinding.layout = tex->layout;

                            binding.bindingArray.push_back(newBinding);
                        }
                        else binding.bindingArray.push_back(VkTextureBindingViewData{});
                    }
                }
                break;
                case TexturePackBindingType::RenderTexture:
                {
                    for (uint32_t j = 0; j < bInfo.arraySize; j++)
                    {
                        auto& tInfo = bInfo.textures[j];

                        if (tInfo.texture)
                        {
                            auto* tex = (VkRenderTexture*)tInfo.texture;

                            if (tInfo.index >= tex->colorAttachmentCount)
                            {
                                if (!tex->depthTexture) throw GENERIC_EXCEPT("[Hyperflow]", "Render texture invalid attachment index!");

                                auto newBinding = VkTextureBindingViewData{};
                                newBinding.view = tex->depthTexture->view;
                                newBinding.layout = tex->depthTexture->layout;

                                binding.bindingArray.push_back(newBinding);
                            }
                            else
                            {
                                const VkTexture* viewTex = tex->colorTextures[tInfo.index];
                                auto newBinding = VkTextureBindingViewData{};
                                newBinding.view = viewTex->view;
                                newBinding.layout = viewTex->layout;

                                binding.bindingArray.push_back(newBinding);
                            }
                        }
                        else binding.bindingArray.push_back(VkTextureBindingViewData{});
                    }
                }
                break;
            }

            bindings[bInfo.bindingIndex] = binding;

            for (auto& b : bindings | std::views::values)
                CreateDescriptorForBinding(b, 0, b.bindingArray.size());
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        for (auto& binding : bindings | std::views::values)
        {
            for (uint32_t i = 0; i < binding.bindingArray.size(); i++)
            {
                auto& bRef = binding.bindingArray.at(i);
                for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
                {
                    if (bRef.descriptorBindings->descriptorMapping != nullptr)
                        FreeDescriptor(GRAPHICS_DATA.imageDescriptorBuffer, bRef.descriptorBindings[k]);
                }
            }
        }

        bindings.clear();
    }

    void SetTextureBinding(VkTexturePack* pack, const inter::rendering::TexturePackBindingUploadInfo_i* bindings, uint32_t bindingCount)
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
                    auto& texBinding = binding.bindingArray.at(j);

                    switch (binding.bindingType)
                    {
                        case TexturePackBindingType::Texture2D:
                        case TexturePackBindingType::Cubemap:
                        {
                            const auto* tex = (VkTexture*)texInfo.texture;
                            texBinding.view = tex->view;
                            texBinding.layout = tex->layout;
                        }
                        break;
                        case TexturePackBindingType::RenderTexture:
                        {
                            auto* tex = (VkRenderTexture*)texInfo.texture;
                            if (texInfo.index >= tex->colorAttachmentCount)
                            {
                                if (!tex->depthTexture) throw GENERIC_EXCEPT("[Hyperflow]", "Render texture invalid attachment index!");

                                const auto dt = tex->depthTexture;
                                texBinding.view = dt->view;
                                texBinding.layout = dt->layout;
                            }
                            else
                            {
                                const auto ct = tex->colorTextures[texInfo.index];
                                texBinding.view = ct->view;
                                texBinding.layout = ct->layout;
                            }
                        }
                        break;
                    }

                    CreateDescriptorForBinding(binding, current.texInfo->offset, current.texInfo->count);
                }

                wasModified = true;
            }

            if (wasModified)
            {
                GRAPHICS_DATA.preAllocBuffers.indices[count] = current.bindingIndex;
                count++;
            }
        }

        if (count == 0) LOG_WARN("Unnecessary set binding call, noting changed");
    }

    void CreateDescriptorForBinding(VkTextureBinding& binding, uint32_t from, uint32_t size)
    {
        const auto sampler = GetSampler(binding.sampler)->sampler;
        const uint32_t end = from + size;
        for (uint32_t i = from; i < end; i++)
        {
            auto& bRef = binding.bindingArray.at(i);
            if (bRef.view == nullptr) continue;

            VkGetImageDescriptorInfo descInfo{};
            descInfo.type = VkDescriptorImageType::CombinedImageSampler;
            descInfo.imageInfo.imageLayout = bRef.layout;
            descInfo.imageInfo.imageView = bRef.view;
            descInfo.imageInfo.sampler = sampler;

            for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
            {
                if (bRef.descriptorBindings->descriptorMapping != nullptr)
                {
                    FreeDescriptor(GRAPHICS_DATA.imageDescriptorBuffer, bRef.descriptorBindings[k]);
                    bRef.descriptorBindings[k] = VkDescriptorLocation{};
                }
                GetImageDescriptorFromBuffer(GRAPHICS_DATA.imageDescriptorBuffer, descInfo, bRef.descriptorBindings[k]);
            }
        }
    }

    void BindTexturePack(const VkRenderer* rn, const inter::rendering::BindResourceInfo_i<VkTexturePack*>& info)
    {
        const auto currentFrame = rn->currentFrame;
        uint32_t bindingCount = 0;

        for (uint32_t i = 0; i < info.objectCount; i++)
        {
            const auto& obj = info.objects[i];
            for (auto& binding : obj->bindings | std::views::values)
            {
                for (uint32_t j = 0; j < binding.bindingArray.size(); j++)
                {
                    const auto& viewData = binding.bindingArray.at(j);
                    const auto& loc = viewData.descriptorBindings[currentFrame];

                    GRAPHICS_DATA.preAllocBuffers.descBindingInfos[bindingCount] = VkDescriptorBufferBindingInfoEXT
                    {
                        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT,
                        .address = loc.address,
                        .usage = loc.usageFlags,
                    };

                    GRAPHICS_DATA.preAllocBuffers.indices[bindingCount] = 0;
                    GRAPHICS_DATA.preAllocBuffers.sizes[bindingCount] = loc.offset; //will be used as offsets.
                    bindingCount++;
                }
            }
        }

        FinishObjectBinding(rn, bindingCount, (VkPipelineBindPoint)info.bindingType, info.setBindingIndex);
    }
}
