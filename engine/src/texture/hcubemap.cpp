#include "hyaml.h"
#include <stb_image.h>
#include "hcubemap.h"
#include "hinternal.h"
#include "hstrconversion.h"
#include "hyperflow.h"

namespace hf
{
    Cubemap::Cubemap(const CubemapCreationInfo& info) :
        folderPath(info.folderPath), desiredChannel(info.desiredChannel), mipLevels(info.mipLevels),
        details(info.details)
    {
        switch (inter::HF.renderingApi.type)
        {
        case RenderingApiType::None: throw GENERIC_EXCEPT("[Hyperflow]", "No rendering api to use the texture for!");
        case RenderingApiType::Vulkan:
        {
            texturePaths[0] = info.texturePaths.front;
            texturePaths[1] = info.texturePaths.back;
            texturePaths[2] = info.texturePaths.up;
            texturePaths[3] = info.texturePaths.down;
            texturePaths[4] = info.texturePaths.right;
            texturePaths[5] = info.texturePaths.left;
        }break;
        case RenderingApiType::Direct3D:
        {
            texturePaths[0] = info.texturePaths.left;
            texturePaths[1] = info.texturePaths.right;
            texturePaths[2] = info.texturePaths.down;
            texturePaths[3] = info.texturePaths.up;
            texturePaths[4] = info.texturePaths.back;
            texturePaths[5] = info.texturePaths.front;
        }break;
        }

        inter::rendering::CreateCubemap_i(this);
    }

    Cubemap::~Cubemap()
    {
        inter::rendering::DestroyCubemap_i(this);
    }

    Ref<Cubemap> Create(const CubemapCreationInfo& info)
    {
        auto cm = MakeRef<Cubemap>(info);
        inter::HF.graphicsResources.cubemaps[(uint64_t)cm.get()] = cm;
        return cm;
    }

    void Destroy(const Ref<Cubemap>& cm)
    {
        inter::rendering::DestroyCubemap_i(cm.get());
        inter::HF.graphicsResources.cubemaps.erase((uint64_t)cm.get());
    }

    void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            inter::rendering::DestroyCubemap_i(pCubemaps[i].get());
            inter::HF.graphicsResources.cubemaps.erase((uint64_t)pCubemaps[i].get());
        }
    }

    bool IsLoaded(const Ref<Cubemap>& cb) { return cb->handle; }

    namespace inter::rendering
    {
        Ref<Cubemap> CreateCubemapAsset_i(const char* assetPath)
        {
            const auto assetLoc = TO_RES_PATH(std::string("cubemaps/") + assetPath) + ".meta";
            std::vector<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

            try
            {
                CubemapCreationInfo info{};
                info.folderPath = FilePath{ .path = assetPath, .isAbsolute = true };

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["desiredChannel"].val();
                    std::string_view vView{v.str, v.len};
                    info.desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
                }

                info.mipLevels = std::stoi(root["mipLevels"].val().str);

                ReadCubemapTexturePaths_i(root["texturePaths"], info.texturePaths);
                ReadTextureDetails_i     (root["details"], info.details);

                return MakeRef<Cubemap>(info);
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Cubemap: %s", assetPath);
                return nullptr;
            }
        }

        bool CreateCubemap_i(Cubemap* cubemap)
        {
            struct TexData
            {
                uint8_t* pixels{};
                uvec3 size{};
            };

#define VALIDATION_CHECK\
            if (!validLoading)\
            {\
                for (uint32_t i = 0; i < 6; i++)\
                    if (textures[i].pixels) stbi_image_free(textures[i].pixels);\
                return false;\
            }

            if (cubemap->handle) return false;

            const auto fullCubemapFolderPath = TO_RES_PATH(std::string("cubemaps/") + cubemap->folderPath.path + "/");
            TexData textures[6]{};
            bool validLoading = true;

            for (uint32_t i = 0; i < 6; i++)
            {
                std::string path = fullCubemapFolderPath + cubemap->texturePaths[i].path;

                if (!utils::FileExists(path.c_str()))
                {
                    LOG_ERROR("[Hyperflow] Unable to find texture: %s", cubemap->texturePaths[i].path.c_str());
                    validLoading = false;
                    break;
                }

                if (HF.renderingApi.type == RenderingApiType::Vulkan) stbi_set_flip_vertically_on_load(false);

                ivec3 size{};
                int32_t texChannels{};
                uint8_t* pixelData = stbi_load(path.c_str(), &size.x, &size.y,
                &texChannels, (int32_t)cubemap->desiredChannel);
                size.z = 1;

                if (pixelData)
                {
                    TexData texData{};
                    texData.pixels = pixelData;
                    texData.size = size;

                    textures[i] = texData;
                }
            }
            VALIDATION_CHECK

            for (uint32_t i = 0; i < 5; i++)
            {
                if (textures[i].size != textures[i + 1].size)
                {
                    LOG_ERROR("[Hyperflow] Cubemap textures must be the same size!");
                    validLoading = false;
                    break;
                }
            }
            VALIDATION_CHECK

            const auto size = textures[0].size;
            const auto texSize = size.x * size.y * size.z * 4;
            const auto allPixels = (uint8_t*)utils::Allocate(6 * texSize);
            for (uint32_t i = 0; i < 6; i++) memcpy(allPixels + i * texSize, textures[i].pixels, texSize);

            TextureCreationInfo_i info{};
            info.type         = TextureType::Tex2D;
            info.viewType     = TextureViewType::TexCube;
            info.flags        = TextureFlags::CubeCompatible;
            info.size         = size;
            info.channel      = TextureChannel::RGBA;
            info.mipLevels    = cubemap->mipLevels;
            info.samples      = MultisampleMode::MSAA_1X;
            info.pTextures    = allPixels;
            info.textureCount = 6;
            info.details      = cubemap->details;

            cubemap->handle = HF.renderingApi.api.CreateTexture(info);

            return true;
        }

        bool DestroyCubemap_i(Cubemap* cubemap)
        {
            if (cubemap->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.textures.push_back(cubemap->handle);
                cubemap->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllCubemaps_i(bool internalOnly)
        {
            for (const auto& cm : std::ranges::views::values(HF.graphicsResources.cubemaps))
                DestroyCubemap_i(cm.get());
            if (!internalOnly) HF.graphicsResources.cubemaps.clear();
        }
    }
}
