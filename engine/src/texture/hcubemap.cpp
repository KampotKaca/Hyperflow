#include "hyaml.h"
#include <stb_image.h>
#include "hcubemap.h"
#include "hinternal.h"
#include "hstrconversion.h"
#include "hyperflow.h"

namespace hf
{
    Cubemap::Cubemap(const CubemapCreationInfo& info) :
        folderPath(info.folderPath), desiredChannel(info.desiredChannel), details(info.details),
        mipLevels(info.mipLevels)
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
            }
            break;
        case RenderingApiType::Direct3D:
            {
                texturePaths[0] = info.texturePaths.left;
                texturePaths[1] = info.texturePaths.right;
                texturePaths[2] = info.texturePaths.down;
                texturePaths[3] = info.texturePaths.up;
                texturePaths[4] = info.texturePaths.back;
                texturePaths[5] = info.texturePaths.front;
            }
            break;
        }

        inter::rendering::CreateCubemap_i(this);
    }

    Cubemap::~Cubemap()
    {
        inter::rendering::DestroyCubemap_i(this);
    }

    bool IsRunning(const Ref<Cubemap>& cb) { return cb->handle; }
    void Destroy(const Ref<Cubemap>& cb)
    {
        if (inter::rendering::DestroyCubemap_i(cb.get()))
            inter::HF.graphicsResources.cubemaps.erase(cb->folderPath);
    }

    Ref<Cubemap> Create(const CubemapCreationInfo& info)
    {
        Ref<Cubemap> cubemap = MakeRef<Cubemap>(info);
        inter::HF.graphicsResources.cubemaps[info.folderPath] = cubemap;
        return cubemap;
    }

    Ref<Cubemap> CreateCubemapAsset(const char* assetPath)
    {
        std::string assetLoc = TO_RES_PATH(std::string("cubemaps/") + assetPath) + ".meta";
        if (!utils::FileExists(assetLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find cubemap meta file: %s", assetPath);
            return nullptr;
        }

        std::vector<char> metadata{};
        if (!utils::ReadFile(assetLoc, true, metadata))
        {
            LOG_ERROR("[Hyperflow] Unable to read cubemap meta: %s", assetPath);
            return nullptr;
        }

        try
        {
            CubemapCreationInfo info
            {
                .folderPath = assetPath,
            };

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                const auto v = root["desiredChannel"].val();
                std::string_view vView{v.str, v.len};
                info.desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
            }

            info.mipLevels = std::stoi(root["mipLevels"].val().str);
            std::string left, right, down, up, back, front;

            {
                const auto v = root["left"].val();
                std::string_view vView{v.str, v.len};
                left = vView;
            }

            {
                const auto v = root["right"].val();
                std::string_view vView{v.str, v.len};
                right = vView;
            }

            {
                const auto v = root["down"].val();
                std::string_view vView{v.str, v.len};
                down = vView;
            }

            {
                const auto v = root["up"].val();
                std::string_view vView{v.str, v.len};
                up = vView;
            }

            {
                const auto v = root["back"].val();
                std::string_view vView{v.str, v.len};
                back = vView;
            }

            {
                const auto v = root["front"].val();
                std::string_view vView{v.str, v.len};
                front = vView;
            }

            info.texturePaths =
            {
                .left = left.c_str(),
                .right = right.c_str(),
                .down = down.c_str(),
                .up = up.c_str(),
                .back = back.c_str(),
                .front = front.c_str()
            };

            utils::ReadTextureDetails(&tree, &root, info.details);

            auto cubemap = Create(info);
            inter::HF.graphicsResources.cubemaps[cubemap->folderPath] = cubemap;
            return cubemap;
        }catch (...)
        {
            LOG_ERROR("[Hyperflow] Error parsing Cubemap: %s", assetPath);
            return nullptr;
        }
    }

    void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto cubemap = pCubemaps[i];
            if (inter::rendering::DestroyCubemap_i(cubemap.get()))
                inter::HF.graphicsResources.cubemaps.erase(cubemap->folderPath);
        }
    }

    namespace inter::rendering
    {
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

            std::string fullCubemapFolderPath = TO_RES_PATH(std::string("cubemaps/") + cubemap->folderPath + "/");
            TexData textures[6]{};
            bool validLoading = true;

            for (uint32_t i = 0; i < 6; i++)
            {
                std::string path = fullCubemapFolderPath + cubemap->texturePaths[i];

                if (!utils::FileExists(path.c_str()))
                {
                    LOG_ERROR("[Hyperflow] Unable to find texture: %s", cubemap->texturePaths[i].c_str());
                    validLoading = false;
                    break;
                }

                if (HF.renderingApi.type == RenderingApiType::Vulkan)
                    stbi_set_flip_vertically_on_load(false);

                ivec3 size{};
                int32_t texChannels{};
                uint8_t* pixelData = stbi_load(path.c_str(), &size.x, &size.y,
                &texChannels, (int32_t)cubemap->desiredChannel);
                size.z = 1;

                if (pixelData)
                {
                    textures[i] =
                    {
                        .pixels = pixelData,
                        .size = size
                    };
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
            auto allPixels = (uint8_t*)utils::Allocate(6 * texSize);
            for (uint32_t i = 0; i < 6; i++) memcpy(allPixels + i * texSize, textures[i].pixels, texSize);

            const TextureCreationInfo info
            {
                .type = TextureType::Tex2D,
                .viewType = TextureViewType::TexCube,
                .flags = TextureFlags::CubeCompatible,
                .size = size,
                .channel = TextureChannel::RGBA,
                .mipLevels = cubemap->mipLevels,
                .samples = 1,
                .pTextures = allPixels,
                .textureCount = 6,
                .details = cubemap->details
            };
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
            for (const auto& cubemap : std::ranges::views::values(HF.graphicsResources.cubemaps))
                DestroyCubemap_i(cubemap.get());
            if (!internalOnly) HF.graphicsResources.cubemaps.clear();
        }
    }
}
