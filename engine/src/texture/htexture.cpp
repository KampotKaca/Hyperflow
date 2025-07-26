#include "hyaml.h"
#include <stb_image.h>
#include "htexture.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hstrconversion.h"

namespace hf
{
    Texture::Texture(const TextureCreationInfo& info) :
        filePath(info.filePath), useAbsolutePath(info.useAbsolutePath), desiredChannel(info.desiredChannel), details(info.details),
        mipLevels(info.mipLevels)
    {
        inter::rendering::CreateTexture_i(this);
    }

    Texture::~Texture()
    {
        inter::rendering::DestroyTexture_i(this);
    }

    bool IsLoaded(const Ref<Texture>& tex) { return tex->handle; }
    void Destroy(const Ref<Texture>& tex)
    {
        if (inter::rendering::DestroyTexture_i(tex.get()))
            inter::HF.graphicsResources.textures.erase(tex->filePath);
    }

    Ref<Texture> Create(const TextureCreationInfo& info)
    {
        Ref<Texture> texture = MakeRef<Texture>(info);
        inter::HF.graphicsResources.textures[texture->filePath] = texture;
        return texture;
    }

    Ref<Texture> CreateTextureAsset(const char* assetPath)
    {
        const auto assetLoc = TO_RES_PATH(std::string("textures/") + assetPath) + ".meta";
        if (!utils::FileExists(assetLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find texture meta file: %s", assetPath);
            return nullptr;
        }

        std::vector<char> metadata{};
        if (!utils::ReadFile(assetLoc, true, metadata))
        {
            LOG_ERROR("[Hyperflow] Unable to read texture meta: %s", assetPath);
            return nullptr;
        }

        try
        {
            TextureCreationInfo info
            {
                .filePath = assetPath,
            };

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                const auto v = root["desiredChannel"].val();
                std::string_view vView{v.str, v.len};
                info.desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
            }

            info.mipLevels = std::stoi(root["mipLevels"].val().str);

            utils::ReadTextureDetails(&tree, &root, info.details);

            auto texture = Create(info);
            inter::HF.graphicsResources.textures[texture->filePath] = texture;
            return texture;
        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Texture: %s", assetPath);
            return nullptr;
        }
    }

    void Destroy(const Ref<Texture>* pTextures, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto tex = pTextures[i];
            if (inter::rendering::DestroyTexture_i(tex.get()))
                inter::HF.graphicsResources.textures.erase(tex->filePath);
        }
    }

    void SubmitAllTextures() { inter::HF.renderingApi.api.SubmitTextureCopyOperations(); }

    namespace inter::rendering
    {
        bool CreateTexture_i(Texture* tex)
        {
            if (tex->handle) return false;

            std::string texLoc{};

            if (tex->useAbsolutePath) texLoc = tex->filePath;
            else texLoc = TO_RES_PATH(std::string("textures/") + tex->filePath);

            if (!utils::FileExists(texLoc.c_str()))
            {
                LOG_ERROR("[Hyperflow] Unable to find texture: %s", tex->filePath.c_str());
                return false;
            }

            if (HF.renderingApi.type == RenderingApiType::Vulkan) stbi_set_flip_vertically_on_load(false);

            ivec3 size{};
            int32_t texChannels{};
            tex->pixelCache = stbi_load(texLoc.c_str(), &size.x, &size.y,
            &texChannels, (int32_t)tex->desiredChannel);
            size.z = 1;
            if (tex->pixelCache)
            {
                const TextureCreationInfo_i creationInfo =
                {
                    .type = TextureType::Tex2D,
                    .viewType = TextureViewType::Tex2D,
                    .size = size,
                    .channel = (TextureChannel)texChannels,
                    .mipLevels = tex->mipLevels,
                    .pTextures = tex->pixelCache,
                    .textureCount = 1,
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
    }

    namespace inter::rendering
    {
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

                window::SetIcons(win, images.data(), images.size());
                for (auto& image : images) stbi_image_free(image.data);
                images.clear();

                return true;
            }

            Fail:
            LOG_ERROR("[Hyperflow] Unable to load image at: %s", folderPath);
            return false;
        }
    }
}