#include "textures.h"

namespace app
{
    AppTextures APP_TEXTURES{};

    void TextureDefineAll()
    {
        hf::TextureLayoutBindingInfo viking_room_binding
        {
            .bindingId = 0,
            .usageFlags = hf::ShaderUsageStage::Default,
            .arraySize = 1,
        };

        hf::TextureLayoutDefinitionInfo default_shader_layout_infp
        {
            .pBindings = &viking_room_binding,
            .bindingCount = 1
        };

        APP_TEXTURES.viking_room_layout = hf::texturelayout::Define(default_shader_layout_infp);

        hf::TextureSamplerDefinitionInfo samplerDefinitionInfo
        {
            .filter = hf::TextureFilter::Bilinear,
            .anisotropicFilter = hf::TextureAnisotropicFilter::X16,
            .repeatMode = hf::TextureRepeatMode::Repeat,
            .useNormalizedCoordinates = true,
            .comparison = hf::ComparisonOperation::None,
            .mipMaps =
            {
                .mode = hf::MipMapMode::Linear,
                .minLod = 0.0f,
                .maxLod = 8.0f,
                .lodBias = 0.0f
            }
        };
        APP_TEXTURES.sampler = hf::texturesampler::Define(samplerDefinitionInfo);
    }

    void TextureLoadAll()
    {
        hf::TextureCreationInfo viking_room_info
        {
            .filePath = "viking_room.png",
            .desiredChannel = hf::TextureChannel::RGBA,
            .mipLevels = 8,
            .details
            {
                .type = hf::TextureType::Tex2D,
                .format = hf::TextureFormat::R8G8B8A8_Srgb,
                .aspectFlags = hf::TextureAspectFlags::Color,
                .tiling = hf::TextureTiling::Optimal,
                .usage = hf::TextureUsageFlags::Sampled,
                .memoryType = hf::BufferMemoryType::Static,
                .finalLayout = hf::TextureResultLayoutType::ShaderReadOnly
            }
        };

        APP_TEXTURES.viking_room = hf::texture::Create(viking_room_info);

        hf::TextureCreationInfo greek_head_info
        {
            .filePath = "greek_head.jpg",
            .desiredChannel = hf::TextureChannel::RGBA,
            .mipLevels = 8,
            .details
            {
                .type = hf::TextureType::Tex2D,
                .format = hf::TextureFormat::R8G8B8A8_Srgb,
                .aspectFlags = hf::TextureAspectFlags::Color,
                .tiling = hf::TextureTiling::Optimal,
                .usage = hf::TextureUsageFlags::Sampled,
                .memoryType = hf::BufferMemoryType::Static,
                .finalLayout = hf::TextureResultLayoutType::ShaderReadOnly
            }
        };
        APP_TEXTURES.greek_head = hf::texture::Create(greek_head_info);

        hf::TexturePackBindingInfo viking_room_binding
        {
            .sampler = APP_TEXTURES.sampler,
            .pTextures = &APP_TEXTURES.viking_room,
            .arraySize = 1
        };

        hf::TexturePackCreationInfo texPackInfo
        {
            .bindingType = hf::RenderBindingType::Graphics,
            .bindingId = 0,
            .setBindingIndex = 1,
            .pBindings = &viking_room_binding,
            .bindingCount = 1,
            .layout = APP_TEXTURES.viking_room_layout,
        };

        APP_TEXTURES.viking_room_pack = hf::texturepack::Create(texPackInfo);
        hf::texture::SubmitAll();

        hf::TexturePackAllocatorCreationInfo texPackAllocInfo
        {
            .pTexturePacks = &APP_TEXTURES.viking_room_pack,
            .texturePackCount = 1
        };

        APP_TEXTURES.allocator = hf::texturepackallocator::Create(texPackAllocInfo);
    }
}