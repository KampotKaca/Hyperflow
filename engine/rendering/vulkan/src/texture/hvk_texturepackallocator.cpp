#include "hvk_texturepackallocator.h"

#include "hvk_texturelayout.h"
#include "hvk_texturepack.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexturePackAllocator::VkTexturePackAllocator(const inter::rendering::TexturePackAllocatorCreationInfo& info)
    {
        VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 0
        };

        uint32_t totalTextureDescriptors = 0;
        uint32_t totalDescriptorSets = info.texturePackCount * FRAMES_IN_FLIGHT;

        for (uint32_t i = 0; i < info.texturePackCount; i++)
        {
            auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
            for (auto& binding : texPack->bindings)
                totalTextureDescriptors += binding.textures.size() * FRAMES_IN_FLIGHT;
        }

        poolSize.descriptorCount = totalTextureDescriptors;

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = totalDescriptorSets,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize
        };

        auto& device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        {
            uint32_t currentIndex = 0;
            for (uint32_t i = 0; i < info.texturePackCount; i++)
            {
                auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
                auto& layout = GetTextureLayout(texPack->layout);

                for (uint32_t k = 0; k < FRAMES_IN_FLIGHT; k++)
                {
                    GRAPHICS_DATA.preAllocBuffers.descLayouts[currentIndex] = layout->layout;
                    currentIndex++;
                }
            }
        }

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = totalDescriptorSets,
            .pSetLayouts = GRAPHICS_DATA.preAllocBuffers.descLayouts,
        };

        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, GRAPHICS_DATA.preAllocBuffers.descriptors));

        {
            for (uint32_t i = 0; i < info.texturePackCount; i++)
            {
                auto* texPack = (VkTexturePack*)info.pTexturePacks[i];
                memcpy(texPack->descriptors, &GRAPHICS_DATA.preAllocBuffers.descriptors[i * FRAMES_IN_FLIGHT], sizeof(VkDescriptorSet) * FRAMES_IN_FLIGHT);
                UpdateTexturePack(texPack, 0, texPack->bindings.size());
            }
        }
    }

    VkTexturePackAllocator::~VkTexturePackAllocator()
    {
        vkDestroyDescriptorPool(GRAPHICS_DATA.defaultDevice->logicalDevice.device, pool, nullptr);
    }
}
