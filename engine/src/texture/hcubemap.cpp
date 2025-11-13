#include "hyaml.h"
#include <stb_image.h>
#include "hcubemap.h"
#include "hinternal.h"
#include "hstrconversion.h"
#include "hyperflow.h"

namespace hf
{
    Cubemap::Cubemap(const CubemapCreationInfo& info) :
    mipLevels(info.mipLevels), size(info.size), details(info.details)
    {
        ir::rdr::TextureCreationInfo_i tInfo{};
        tInfo.type         = ir::rdr::TextureType::Tex2D;
        tInfo.viewType     = ir::rdr::TextureViewType::TexCube;
        tInfo.flags        = ir::rdr::TextureFlags::CubeCompatible;
        tInfo.size         = info.size;
        tInfo.channel      = TextureChannel::RGBA;
        tInfo.mipLevels    = mipLevels;
        tInfo.samples      = MultisampleMode::MSAA_1X;
        tInfo.pTextures    = info.data;
        tInfo.textureCount = 6;
        tInfo.details      = details;

        handle = ir::HF.renderingApi.api.CreateTexture(tInfo);
    }

    Cubemap::~Cubemap()
    {
        ir::rdr::DestroyCubemap_i(this);
    }

    Ref<Cubemap> Create(const CubemapCreationInfo& info)
    {
        auto cm = MakeRef<Cubemap>(info);
        ir::HF.graphicsResources.cubemaps[(uint64_t)cm.get()] = cm;
        return cm;
    }

    void Destroy(const Ref<Cubemap>& cm)
    {
        ir::rdr::DestroyCubemap_i(cm.get());
        ir::HF.graphicsResources.cubemaps.erase((uint64_t)cm.get());
    }

    void Destroy(const Ref<Cubemap>* pCubemaps, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            ir::rdr::DestroyCubemap_i(pCubemaps[i].get());
            ir::HF.graphicsResources.cubemaps.erase((uint64_t)pCubemaps[i].get());
        }
    }

    bool IsLoaded(const Ref<Cubemap>& cb) { return cb->handle; }

    namespace ir::rdr
    {
        Ref<Cubemap> CreateCubemapAsset_i(const char* assetPath)
        {
            const auto assetLoc = TO_RES_PATH(std::string("cubemaps/") + assetPath) + ".meta";
            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            CubemapCreationInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            TextureChannel desiredChannel;
            {
                auto node = root["desiredChannel"];
                if (node.readable())
                {
                    const auto v = node.val();
                    std::string_view vView{v.str, v.len};
                    desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
                }
                else
                {
                    log_warn_s("[Hyperflow] Cubemap %s has invalid desiredChannel!", assetPath);
                    desiredChannel = TextureChannel::RGBA;
                }
            }

            {
                auto node = root["mipLevels"];
                if (node.readable()) info.mipLevels = std::stoi(node.val().str);
                else
                {
                    info.mipLevels = 1;
                    log_warn_s("[Hyperflow] Cubemap %s has invalid mipLevels!", assetPath);
                }
            }

            {
                auto node = root["details"];
                if (!node.readable() || !ReadTextureDetails_i(node, info.details))
                    log_warn_s("[Hyperflow] Cubemap %s has invalid details!", assetPath);
            }

            CubemapTexturePaths texPaths{};
            {
                auto node = root["texturePaths"];
                if (!node.readable() || !ReadCubemapTexturePaths_i(node, texPaths))
                    log_warn_s("[Hyperflow] Cubemap %s has invalid texturePaths!", assetPath);
            }

            const auto fullCubemapFolderPath = TO_RES_PATH(std::string("cubemaps/") + assetPath + "/");
            std::string texturePaths[6]{};

            switch (HF.renderingApi.type)
            {
            case RenderingApiType::None:
                log_fatal("[Hyperflow]", "No rendering api to use the texture for!");
                abort();
            case RenderingApiType::Vulkan:
            {
                texturePaths[0] = texPaths.front;
                texturePaths[1] = texPaths.back;
                texturePaths[2] = texPaths.up;
                texturePaths[3] = texPaths.down;
                texturePaths[4] = texPaths.right;
                texturePaths[5] = texPaths.left;
            }break;
            case RenderingApiType::Direct3D:
            {
                texturePaths[0] = texPaths.left;
                texturePaths[1] = texPaths.right;
                texturePaths[2] = texPaths.down;
                texturePaths[3] = texPaths.up;
                texturePaths[4] = texPaths.back;
                texturePaths[5] = texPaths.front;
            }break;
            }

            uint8_t* pixels = nullptr;
            size_t pixelOffset = 0;
            uvec3 textureSize{};

            for (uint32_t i = 0; i < 6; i++)
            {
                std::string path = fullCubemapFolderPath + texturePaths[i];
                hassert(utils::FileExists(path.c_str()), "[Hyperflow] Unable to find Cubemap %s texture: %s", assetPath, texturePaths[i].c_str())

                if (HF.renderingApi.type == RenderingApiType::Vulkan) stbi_set_flip_vertically_on_load(false);

                ivec3 size{};
                int32_t texChannels{};
                uint8_t* pixelData = stbi_load(path.c_str(), &size.x, &size.y, &texChannels, (int32_t)desiredChannel);
                size.z = 1;

                if (i == 0 && size.x > 0 && size.y > 0)
                {
                    pixels = (uint8_t*)utils::Alloc(size.x * size.y * size.z * 4 * 6);
                    pixelOffset = 0;
                    textureSize = size;
                }

                hassert(i == 0 || textureSize == (uvec3)size, "[Hyperflow] Cubemap %s textures must be the same size!", assetPath)
                hassert(pixelData, "[Hyperflow] Cubemap %s Unable to load texture %s!", assetPath, texturePaths[i].c_str());

                auto dataSize = size.x * size.y * size.z * 4;
                memcpy(pixels + pixelOffset, pixelData, dataSize);
                pixelOffset += dataSize;
                stbi_image_free(pixelData);
            }

            info.size = textureSize;
            info.data = pixels;

            auto cubemap = MakeRef<Cubemap>(info);
            if (pixels) utils::Free(pixels);
            return cubemap;
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
