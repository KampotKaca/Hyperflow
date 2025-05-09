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

        std::vector<VkDescriptorPoolSize> poolSizes{};
        for (uint32_t i = 0; i < (uint32_t)TextureLayoutType::Count; i++)
        {
            if (entryCounts[i] > 0)
                poolSizes.emplace_back((VkDescriptorType)i, entryCounts[i]);
        }

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = descriptorCount,
            .poolSizeCount = (uint32_t)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        std::vector<VkDescriptorSetLayout> layouts(descriptorCount);
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
                        layouts[currentIndex] = layout.layout;
                        currentIndex++;
                    }
                }
            }
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = (uint32_t)layouts.size(),
            .pSetLayouts = layouts.data(),
        };

        std::vector<VkDescriptorSet> descriptorSets(layouts.size());
        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));

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
                        auto descriptorSet = descriptorSets[currentIndex];
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
