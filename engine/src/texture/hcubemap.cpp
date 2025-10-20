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
        inter::rendering::TextureCreationInfo_i tInfo{};
        tInfo.type         = inter::rendering::TextureType::Tex2D;
        tInfo.viewType     = inter::rendering::TextureViewType::TexCube;
        tInfo.flags        = inter::rendering::TextureFlags::CubeCompatible;
        tInfo.size         = info.size;
        tInfo.channel      = TextureChannel::RGBA;
        tInfo.mipLevels    = mipLevels;
        tInfo.samples      = MultisampleMode::MSAA_1X;
        tInfo.pTextures    = info.data;
        tInfo.textureCount = 6;
        tInfo.details      = details;

        handle = inter::HF.renderingApi.api.CreateTexture(tInfo);
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
                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                TextureChannel desiredChannel;
                {
                    const auto v = root["desiredChannel"].val();
                    std::string_view vView{v.str, v.len};
                    desiredChannel = STRING_TO_TEXTURE_CHANNEL(vView);
                }

                info.mipLevels = std::stoi(root["mipLevels"].val().str);
                ReadTextureDetails_i(root["details"], info.details);

                CubemapTexturePaths texPaths{};
                ReadCubemapTexturePaths_i(root["texturePaths"], texPaths);

                const auto fullCubemapFolderPath = TO_RES_PATH(std::string("cubemaps/") + assetPath + "/");
                FilePath texturePaths[6]{};

                switch (HF.renderingApi.type)
                {
                case RenderingApiType::None: throw GENERIC_EXCEPT("[Hyperflow]", "No rendering api to use the texture for!");
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

                bool validLoading = true;
                uint8_t* pixels = nullptr;
                size_t pixelOffset = 0;
                uvec3 textureSize{};

                for (uint32_t i = 0; i < 6; i++)
                {
                    std::string path = fullCubemapFolderPath + texturePaths[i].path;

                    if (!utils::FileExists(path.c_str()))
                    {
                        LOG_ERROR("[Hyperflow] Unable to find texture: %s", texturePaths[i].path.c_str());
                        validLoading = false;
                        break;
                    }

                    if (HF.renderingApi.type == RenderingApiType::Vulkan) stbi_set_flip_vertically_on_load(false);

                    ivec3 size{};
                    int32_t texChannels{};
                    uint8_t* pixelData = stbi_load(path.c_str(), &size.x, &size.y,
                    &texChannels, (int32_t)desiredChannel);
                    size.z = 1;

                    if (i == 0 && size.x > 0 && size.y > 0)
                    {
                        pixels = (uint8_t*)utils::Allocate(size.x * size.y * size.z * 4 * 6);
                        pixelOffset = 0;
                        textureSize = size;
                    }

                    if (i != 0 && textureSize != (uvec3)size)
                    {
                        LOG_ERROR("[Hyperflow] Cubemap textures must be the same size!");
                        validLoading = false;
                        break;
                    }

                    if (pixelData)
                    {
                        auto dataSize = size.x * size.y * size.z * 4;
                        memcpy(pixels + pixelOffset, pixelData, dataSize);
                        pixelOffset += dataSize;
                        stbi_image_free(pixelData);
                    }
                    else
                    {
                        LOG_ERROR("[Hyperflow] Unable to load cubemap texture!");
                        validLoading = false;
                        break;
                    }
                }

                if (!validLoading)
                {
                    if (pixels) utils::Deallocate(pixels);
                    return nullptr;
                }

                info.size = textureSize;
                info.data = pixels;

                return MakeRef<Cubemap>(info);
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Cubemap: %s", assetPath);
                return nullptr;
            }
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
