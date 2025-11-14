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
            const ir::rdr::TextureCreationInfo_i creationInfo =
            {
                .type = ir::rdr::TextureType::Tex2D,
                .viewType = ir::rdr::TextureViewType::Tex2D,
                .size = size,
                .channel = channels,
                .mipLevels = mipLevels,
                .pTextures = info.pixels,
                .textureCount = 1,
                .details = details
            };

            handle = ir::HF.renderingApi.api.CreateTexture(creationInfo);
        }
    }

    Texture::~Texture()
    {
        ir::rdr::DestroyTexture_i(this);
    }

    Ref<Texture> Create(const TextureCreationInfo& info)
    {
        auto tex = MakeRef<Texture>(info);
        ir::HF.graphicsResources.textures[(uint64_t)tex.get()] = tex;
        return tex;
    }

    void Destroy(const Ref<Texture>& tex)
    {
        ir::rdr::DestroyTexture_i(tex.get());
        ir::HF.graphicsResources.textures.erase((uint64_t)tex.get());
    }

    void Destroy(const Ref<Texture>* pTextures, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            ir::rdr::DestroyTexture_i(pTextures[i].get());
            ir::HF.graphicsResources.textures.erase((uint64_t)pTextures[i].get());
        }
    }

    bool IsLoaded(const Ref<Texture>& tex) { return tex->handle; }
    void SubmitAllTextures() { ir::HF.renderingApi.api.SubmitTextureCopyOperations(); }

    namespace ir::rdr
    {
        Ref<Texture> CreateTextureAsset_i(const char* assetPath)
        {
            const auto assetLoc = TO_RES_PATH(std::string("textures/") + assetPath) + ".meta";
            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            TextureCreationInfo info{};
            TextureChannel desiredChannel;

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            if (!YamlGetIf_i(root, "desiredChannel", desiredChannel)) log_warn_s("[Hyperflow] Texture %s has invalid desiredChannel!", assetPath);
            if (!YamlGetIf_i(root, "mipLevels", info.mipLevels)) log_warn_s("[Hyperflow] Texture %s has invalid mipLevels!", assetPath);
            if (!YamlGetIf_i(root, "details", info.details)) log_warn_s("[Hyperflow] Texture %s has invalid details!", assetPath);

            const std::string texLoc = TO_RES_PATH(std::string("textures/") + assetPath);
            hassert(utils::FileExists(texLoc.c_str()), "[Hyperflow] Unable to find texture: %s", assetPath)

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
                List<win::Image> images{};
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
                                log_error("[Hyperflow] Invalid image, unable to load at location: %s", entry.path().string().c_str());
                                continue;
                            }

                            images.emplace_back(size, pixels);
                        }
                    }
                }

                win::SetIcons(win, images.data(), (uint32_t)images.size());
                for (const auto& image : images) stbi_image_free(image.data);
                images.clear();

                return true;
            }

            log_error("[Hyperflow] Unable to load image at: %s", folderPath);
            return false;
        }
    }
}