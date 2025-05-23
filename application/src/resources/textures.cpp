#include "resources/textures.h"
#include "resources/texturesamplers.h"
#include "resources/texturelayouts.h"

namespace app
{
    AppTextures APP_TEXTURES{};

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
    }
}
