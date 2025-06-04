#include "hyaml.h"
#include "hcubemap.h"
#include "hinternal.h"
#include "hstrconversion.h"
#include "hyperflow.h"

namespace hf
{
    Cubemap::Cubemap(const CubemapCreationInfo& info)
    {

    }

    Cubemap::~Cubemap()
    {

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

        }

        bool DestroyCubemap_i(Cubemap* cubemap)
        {
            bool state = false;
            for (auto& texture : cubemap->textures) state = state || DestroyTexture_i(texture.get());
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
