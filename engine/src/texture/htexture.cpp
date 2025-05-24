#include <stb_image.h>
#include "htexture.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    Texture::Texture(const TextureCreationInfo& info) :
        filePath(info.filePath), desiredChannel(info.desiredChannel), details(info.details),
        mipLevels(info.mipLevels)
    {
        inter::rendering::CreateTexture_i(this);
    }

    Texture::~Texture()
    {
        inter::rendering::DestroyTexture_i(this);
    }

    namespace texture
    {
        Ref<Texture> Create(const TextureCreationInfo& info)
        {
            Ref<Texture> texture = MakeRef<Texture>(info);
            inter::HF.graphicsResources.textures[(uint64_t)texture.get()] = texture;
            return texture;
        }

        void Destroy(const Ref<Texture>& texture)
        {
            if (inter::rendering::DestroyTexture_i(texture.get()))
                inter::HF.graphicsResources.textures.erase((uint64_t)texture.get());
        }

        void Destroy(const Ref<Texture>* pTextures, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto tex = pTextures[i];
                if (inter::rendering::DestroyTexture_i(tex.get()))
                    inter::HF.graphicsResources.textures.erase((uint64_t)tex.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& tex : std::ranges::views::values(inter::HF.graphicsResources.textures))
                inter::rendering::DestroyTexture_i(tex.get());
            if (!internalOnly) inter::HF.graphicsResources.textures.clear();
        }

        bool IsRunning(const Ref<Texture>& texture) { return texture->handle; }
        void SubmitAll() { inter::HF.renderingApi.api.SubmitTextureCopyOperations(); }
    }

    namespace inter::rendering
    {
        bool CreateTexture_i(Texture* tex)
        {
            if (tex->handle) return false;

            std::string texLoc = TO_RES_PATH(std::string("textures/") + tex->filePath);

            if (!utils::FileExists(texLoc.c_str()))
            {
                LOG_ERROR("[Hyperflow] Unable to find texture: %s", tex->filePath.c_str());
                return false;
            }

            ivec3 size{};
            int32_t texChannels{};
            tex->pixelCache = stbi_load(texLoc.c_str(), &size.x, &size.y,
            &texChannels, (int32_t)tex->desiredChannel);
            size.z = 1;
            if (tex->pixelCache)
            {
                TextureCreationInfo creationInfo =
                {
                    .size = size,
                    .channel = (TextureChannel)texChannels,
                    .mipLevels = tex->mipLevels,
                    .data = tex->pixelCache,
                    .details = tex->details
                };

                tex->handle = HF.renderingApi.api.CreateTexture(creationInfo);
            }
            else
            {
                LOG_ERROR("[Hyperflow] Unable to load texture: %s", tex->filePath.c_str());
                return false;
            }

            return true;
        }

        bool DestroyTexture_i(Texture* tex)
        {
            if (tex->handle)
            {
                HF.renderingApi.api.DestroyTexture(tex->handle);
                tex->handle = nullptr;
                return true;
            }
            return false;
        }
    }

    namespace texturesampler
    {
        TextureSampler Define(const TextureSamplerDefinitionInfo& info)
        {
            return inter::HF.renderingApi.api.DefineTextureSampler(info);
        }
    }

    namespace texturelayout
    {
        TextureLayout Define(const TextureLayoutDefinitionInfo& info)
        {
            return inter::HF.renderingApi.api.DefineTextureLayout(info);
        }
    }
}