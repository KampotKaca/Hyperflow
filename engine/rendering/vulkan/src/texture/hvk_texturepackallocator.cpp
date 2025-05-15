#include "hvk_texturepackallocator.h"

#include "hvk_texturelayout.h"
#include "hvk_texturepack.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexturePackAllocator::VkTexturePackAllocator(const inter::rendering::TexturePackAllocatorCreationInfo& info)
    {
        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        uint32_t textureBindingCount = 0;
        for (uint32_t i = 0; i < info.texturePackCount; i++)
            textureBindingCount += ((VkTexturePack*)info.pTexturePacks[i])->bindings.size();

        uint32_t entryCounts[(uint32_t)TextureLayoutType::Count]{};

        uint32_t descriptorCount = textureBindingCount * FRAMES_IN_FLIGHT;
        for (uint32_t i = 0; i < info.texturePackCount; ++i)
        {
            auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
            auto& layout = GetTextureLayout(texPack->layout);
            for (uint32_t j = 0; j < texPack->bindings.size(); ++j)
                entryCounts[(uint32_t)layout.bindingInfos[j].type] += FRAMES_IN_FLIGHT;
        }

        uint32_t poolSize = 0;
        for (uint32_t i = 0; i < (uint32_t)TextureLayoutType::Count; i++)
        {
            if (entryCounts[i] > 0)
            {
                GRAPHICS_DATA.preAllocBuffers.descPoolSizes[poolSize] =
                    VkDescriptorPoolSize((VkDescriptorType)i, entryCounts[i]);
                poolSize++;
            }
        }

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = descriptorCount,
            .poolSizeCount = poolSize,
            .pPoolSizes = GRAPHICS_DATA.preAllocBuffers.descPoolSizes
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        {
            uint32_t currentIndex = 0;
            for (uint32_t i = 0; i < info.texturePackCount; i++)
            {
                auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
                auto& layout = GetTextureLayout(texPack->layout);

                for (uint32_t j = 0; j < texPack->bindings.size(); j++)
                {
                    for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
                    {
                        GRAPHICS_DATA.preAllocBuffers.descLayouts[currentIndex] = layout.layout;
                        currentIndex++;
                    }
                }
            }
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = descriptorCount,
            .pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts,
        };

        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, GRAPHICS_DATA.preAllocBuffers.descriptors));

        {
            uint32_t currentIndex = 0;
            for (uint32_t i = 0; i < info.texturePackCount; i++)
            {
                auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
                for (uint32_t j = 0; j < texPack->bindings.size(); j++)
                {
                    auto& binding = texPack->bindings[j];
                    for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
                    {
                        auto descriptorSet = GRAPHICS_DATA.preAllocBuffers.descriptors[currentIndex];
                        binding.descriptors[k] = descriptorSet;
                        texPack->descriptorCache[k][j] = descriptorSet;
                        currentIndex++;
                    }

                    if (binding.sampler == 0) continue;

                    bool isValid = binding.textures.size() > 0;
                    for (uint32_t k = 0; k < binding.textures.size(); k++)
                    {
                        if (!binding.textures[k])
                        {
                            isValid = false;
                            break;
                        }
                    }
                    if (isValid) UpdateTextureBinding(texPack, j, 0, binding.textures.size());
                }
            }
        }
    }

    VkTexturePackAllocator::~VkTexturePackAllocator()
    {
        vkDestroyDescriptorPool(GRAPHICS_DATA.defaultDevice->logicalDevice.device, pool, nullptr);
    }
}
