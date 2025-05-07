#include "hvk_texturepack.h"
#include "hvk_graphics.h"

namespace hf
{
    VkTexturePack::VkTexturePack(const inter::rendering::TexturePackCreationInfo& info)
    {
        textures = std::vector<VkPackedTexture>(info.textureCount);
        bindingId = info.bindingId;

        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;
        std::vector<VkDescriptorPoolSize> poolSizes(info.textureCount);
        uint32_t descriptorCount = info.textureCount * FRAMES_IN_FLIGHT;
        for (uint32_t i = 0; i < info.textureCount; ++i)
        {
            VkPackedTexture texture
            {
                .texture = (VkTexture*)info.pTextures[i]
            };

            VkDescriptorSetLayoutBinding samplerLayoutBinding
            {
                .binding = bindingId + i,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = (VkShaderStageFlags)info.usageStage,
                .pImmutableSamplers = nullptr
            };

            VkDescriptorSetLayoutCreateInfo layoutInfo
            {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = 1,
                .pBindings = &samplerLayoutBinding
            };

            VK_HANDLE_EXCEPT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &texture.layout));

            poolSizes[i] =
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = FRAMES_IN_FLIGHT
            };

            textures[i] = texture;
        }

        std::vector<VkDescriptorSetLayout> layouts(descriptorCount);

        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            for (uint32_t j = 0; j < FRAMES_IN_FLIGHT; j++)
                layouts[i * FRAMES_IN_FLIGHT + j] = textures[i].layout;
        }

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = descriptorCount,
            .poolSizeCount = (uint32_t)poolSizes.size(),
            .pPoolSizes = poolSizes.data()
        };

        VK_HANDLE_EXCEPT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool));

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = (uint32_t)layouts.size(),
            .pSetLayouts = layouts.data(),
        };
        std::vector<VkDescriptorSet> descriptorSets(layouts.size());
        VK_HANDLE_EXCEPT(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));

        for (uint32_t i = 0; i < info.textureCount; i++)
        {
            auto& texture = textures[i];
            memcpy(texture.descriptors, &descriptorSets[i * FRAMES_IN_FLIGHT],
                sizeof(VkDescriptorSet) * FRAMES_IN_FLIGHT);

            SetupTexture(texture.texture, bindingId + i, texture.descriptors);
        }
    }

    VkTexturePack::~VkTexturePack()
    {
        auto device = GRAPHICS_DATA.defaultDevice->logicalDevice.device;

        vkDestroyDescriptorPool(device, pool, nullptr);
        for (auto& texture : textures) vkDestroyDescriptorSetLayout(device, texture.layout, nullptr);
        textures.clear();
    }
}
