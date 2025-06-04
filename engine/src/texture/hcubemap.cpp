#include "hyaml.h"
#include "hcubemap.h"
#include "htexture.h"
#include "hinternal.h"
#include "hstrconversion.h"
#include "hyperflow.h"

namespace hf
{
    Cubemap::Cubemap(const CubemapCreationInfo& info) :
        folderPath(info.folderPath), desiredChannel(info.desiredChannel), details(info.details),
        mipLevels(info.mipLevels)
    {
        texturePaths[0] = info.texturePaths.left;
        texturePaths[1] = info.texturePaths.right;
        texturePaths[2] = info.texturePaths.down;
        texturePaths[3] = info.texturePaths.up;
        texturePaths[4] = info.texturePaths.back;
        texturePaths[5] = info.texturePaths.front;

        TextureCreationInfo texInfo
        {
            .useAbsolutePath = true,
            .desiredChannel = desiredChannel,
            .mipLevels = mipLevels,
            .details = details
        };

        std::string fullCubemapFolderPath = TO_RES_PATH(std::string("cubemaps/") + folderPath + "/");
        for (uint32_t i = 0; i < 6; i++)
        {
            std::string path = fullCubemapFolderPath + texturePaths[i];
            texInfo.filePath = path.c_str();

            textures[i] = MakeRef<Texture>(texInfo);
        }
    }

    Cubemap::~Cubemap()
    {
        inter::rendering::DestroyCubemap_i(this);
    }

    namespace cubemap
    {
        Ref<Cubemap> Create(const CubemapCreationInfo& info)
        {
            Ref<Cubemap> cubemap = MakeRef<Cubemap>(info);
            inter::HF.graphicsResources.cubemaps[info.folderPath] = cubemap;
            return cubemap;
        }

        Ref<Cubemap> Create(const char* assetPath)
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

        void Destroy(const Ref<Cubemap>& cubemap)
        {
            if (inter::rendering::DestroyCubemap_i(cubemap.get()))
                inter::HF.graphicsResources.cubemaps.erase(cubemap->folderPath);
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
    }

    namespace inter::rendering
    {
        bool CreateCubemap_i(Cubemap* cubemap)
        {
            bool state = false;
            for (auto& texture : cubemap->textures)
            {
                const auto result = CreateTexture_i(texture.get());
                state = state || result;
            }
            return state;
        }

        bool DestroyCubemap_i(Cubemap* cubemap)
        {
            bool state = false;
            for (auto& texture : cubemap->textures)
            {
                const auto result = DestroyTexture_i(texture.get());
                state = state || result;
            }
            return state;
        }

        void DestroyAllCubemaps_i(bool internalOnly)
        {
            for (const auto& cubemap : std::ranges::views::values(HF.graphicsResources.cubemaps))
                DestroyCubemap_i(cubemap.get());
            if (!internalOnly) HF.graphicsResources.cubemaps.clear();
        }
    }
}
