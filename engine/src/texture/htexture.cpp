#include "hyaml.h"
#include <stb_image.h>
#include "htexture.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hstrconversion.h"

namespace hf
{
    Texture::Texture(const TextureCreationInfo& info) :
        channels(info.channels), details(info.details), mipLevels(info.mipLevels),
        size(info.size)
    {
        if (info.pixels)
        {
            const inter::rendering::TextureCreationInfo_i creationInfo =
            {
                .type = inter::rendering::TextureType::Tex2D,
                .viewType = inter::rendering::TextureViewType::Tex2D,
                .size = size,
                .channel = channels,
                .mipLevels = mipLevels,
                .pTextures = info.pixels,
                .textureCount = 1,
                .details = details
            };

            handle = inter::HF.renderingApi.api.CreateTexture(creationInfo);
        }
    }

    Texture::~Texture()
    {
        inter::rendering::DestroyTexture_i(this);
    }

    Ref<Texture> Create(const TextureCreationInfo& info)
    {
        auto tex = MakeRef<Texture>(info);
        inter::HF.graphicsResources.textures[(uint64_t)tex.get()] = tex;
        return tex;
    }

    void Destroy(const Ref<Texture>& tex)
    {
        inter::rendering::DestroyTexture_i(tex.get());
        inter::HF.graphicsResources.textures.erase((uint64_t)tex.get());
    }

    void Destroy(const Ref<Texture>* pTextures, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            inter::rendering::DestroyTexture_i(pTextures[i].get());
            inter::HF.graphicsResources.textures.erase((uint64_t)pTextures[i].get());
        }
    }

    bool IsLoaded(const Ref<Texture>& tex) { return tex->handle; }
    void SubmitAllTextures() { inter::HF.renderingApi.api.SubmitTextureCopyOperations(); }

    namespace inter::rendering
    {
        Ref<Texture> CreateTextureAsset_i(const char* assetPath)
        {
            const auto assetLoc = TO_RES_PATH(std::string("textures/") + assetPath) + ".meta";
            List<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

            try
            {
                TextureCreationInfo info{};
                TextureChannel desiredChannel;

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["desiredChannel"].val();
                    const std::string_view vView{v.str, v.len};
                    desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
                }

                info.mipLevels = (uint32_t)std::stoi(root["mipLevels"].val().str);
                ReadTextureDetails_i(root["details"], info.details);

                const std::string texLoc = TO_RES_PATH(std::string("textures/") + assetPath);

                if (!utils::FileExists(texLoc.c_str()))
                {
                    LOG_ERROR("[Hyperflow] Unable to find texture: %s", assetPath);
                    return nullptr;
                }

                if (HF.renderingApi.type == RenderingApiType::Vulkan) stbi_set_flip_vertically_on_load(false);

                ivec3 size{};
                int32_t texChannels{};
                info.pixels = stbi_load(texLoc.c_str(), &size.x, &size.y, &texChannels, (int32_t)desiredChannel);
                size.z = 1;

                info.channels = (TextureChannel)texChannels;
                info.size = size;

                auto tex = MakeRef<Texture>(info);
                if (info.pixels) stbi_image_free(info.pixels);
                return tex;
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Texture: %s", assetPath);
                return nullptr;
            }
        }

        bool DestroyTexture_i(Texture* tex)
        {
            if (tex->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.textures.push_back(tex->handle);
                tex->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllTextures_i(bool internalOnly)
        {
            for (const auto& tex : std::ranges::views::values(HF.graphicsResources.textures))
                DestroyTexture_i(tex.get());
            if (!internalOnly) HF.graphicsResources.textures.clear();
        }

        bool SetWindowIcons_i(const Window* win, const char* folderPath)
        {
            std::filesystem::path fPath = TO_RES_PATH(folderPath);
            if (std::filesystem::exists(fPath) && std::filesystem::is_directory(fPath))
            {
                std::vector<window::Image> images{};
                for (const auto& entry : std::filesystem::recursive_directory_iterator(fPath))
                {
                    if (entry.is_regular_file())
                    {
                        auto ext = entry.path().extension().string();
                        std::ranges::transform(ext, ext.begin(), ::tolower);

                        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
                        {
                            ivec3 size{};
                            int32_t texChannels{};
                            stbi_set_flip_vertically_on_load(false);
                            unsigned char* pixels = stbi_load(entry.path().string().c_str(), &size.x, &size.y,
                            &texChannels, (uint32_t)TextureChannel::RGBA);
                            if (!pixels)
                            {
                                LOG_ERROR("[Hyperflow] Invalid image, unable to load at location: %s", entry.path().string().c_str());
                                continue;
                            }

                            images.emplace_back(size, pixels);
                        }
                    }
                }

                window::SetIcons(win, images.data(), (uint32_t)images.size());
                for (const auto& image : images) stbi_image_free(image.data);
                images.clear();

                return true;
            }

            LOG_ERROR("[Hyperflow] Unable to load image at: %s", folderPath);
            return false;
        }
    }
}