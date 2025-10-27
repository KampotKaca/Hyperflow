#include "hasset.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    static char PATH_NAME_BUFFER[256]{};

    static const char* convertAssetPath(uint32_t size, AssetType type)
    {
        switch (type)
        {
        case AssetType::Model:         memcpy(&PATH_NAME_BUFFER[size], "_mod", 4); break;
        case AssetType::Texture:       memcpy(&PATH_NAME_BUFFER[size], "_tex", 4); break;
        case AssetType::Cubemap:       memcpy(&PATH_NAME_BUFFER[size], "_cub", 4); break;
        case AssetType::AudioClip:     memcpy(&PATH_NAME_BUFFER[size], "_adc", 4); break;
        case AssetType::ShaderLibrary: memcpy(&PATH_NAME_BUFFER[size], "_shl", 4); break;
        case AssetType::Shader:        memcpy(&PATH_NAME_BUFFER[size], "_sha", 4); break;
        case AssetType::TexturePack:   memcpy(&PATH_NAME_BUFFER[size], "_txp", 4); break;
        default: LOG_ERROR("%s", "Invalid asset type, cannot be destroyed"); break;
        }
        PATH_NAME_BUFFER[size + 4] = '\0';
        return PATH_NAME_BUFFER;
    }

    static const char* convertAssetPath(const std::string_view assetPath, AssetType type)
    {
        memcpy(PATH_NAME_BUFFER, assetPath.data(), assetPath.size());
        return convertAssetPath(assetPath.size(), type);
    }

    static const char* convertAssetPath(const char* assetPath, AssetType type)
    {
        const uint32_t strLength = std::strlen(assetPath);
        memcpy(PATH_NAME_BUFFER, assetPath, strLength);
        return convertAssetPath(strLength, type);
    }

    void Asset::Increment(const char* assetPath)
    {
        if (usageCount == 0) Create(assetPath);
        usageCount++;
    }

    void Asset::Decrement()
    {
        usageCount--;
        usageCount = std::max(0u, usageCount);

        if (usageCount == 0) Destroy();
    }

    void Asset::Destroy()
    {
        if (asset)
        {
            switch (type)
            {
            case AssetType::Model:         ir::rdr::DestroyModel_i(Cast<Model>(asset).get());                     break;
            case AssetType::Texture:       ir::rdr::DestroyTexture_i(Cast<Texture>(asset).get());             break;
            case AssetType::Cubemap:       ir::rdr::DestroyCubemap_i(Cast<Cubemap>(asset).get());                 break;
            case AssetType::AudioClip:     ir::audio::DestroyAudioClip_i(Cast<AudioClip>(asset).get());                 break;
            case AssetType::ShaderLibrary: ir::rdr::DestroyShaderLibrary_i(Cast<ShaderLibrary>(asset).get()); break;
            case AssetType::Shader:        ir::rdr::DestroyShader_i(Cast<Shader>(asset).get());                   break;
            case AssetType::TexturePack:   ir::rdr::DestroyTexturePack_i(Cast<TexturePack>(asset).get());         break;
            default: LOG_ERROR("%s", "Invalid asset type, cannot be destroyed"); break;
            }

            asset = nullptr;
        }
    }

    void Asset::Create(const char* assetPath)
    {
        if (!asset)
        {
            switch (type)
            {
            case AssetType::Model:         asset = ir::rdr::CreateModelAsset_i(assetPath);         break;
            case AssetType::Texture:       asset = ir::rdr::CreateTextureAsset_i(assetPath);       break;
            case AssetType::Cubemap:       asset = ir::rdr::CreateCubemapAsset_i(assetPath);       break;
            case AssetType::AudioClip:     asset = ir::audio::CreateAudioClipAsset_i(assetPath);         break;
            case AssetType::ShaderLibrary: asset = ir::rdr::CreateShaderLibraryAsset_i(assetPath); break;
            case AssetType::Shader:        asset = ir::rdr::CreateShaderAsset_i(assetPath);        break;
            case AssetType::TexturePack:   asset = ir::rdr::CreateTexPackAsset_i(assetPath);       break;
            default: LOG_ERROR("%s", "Invalid asset type, cannot be loaded"); break;
            }
        }
    }

    Ref<void> CreateAsset(const char* assetPath, AssetType type)
    {
        auto& assets = ir::HF.graphicsResources.assets;
        auto [it, inserted] = assets.try_emplace(convertAssetPath(assetPath, type));

        if (inserted) it->second.type = type;
        else if (it->second.type != type) LOG_ERROR("%s", "Asset type mismatch");

        it->second.Increment(assetPath);
        return it->second.asset;
    }

    template <typename T>
    static void destroyAsset(T assetPath, AssetType type)
    {
        auto it = ir::HF.graphicsResources.assets.find(convertAssetPath(assetPath, type));
        if (it == ir::HF.graphicsResources.assets.end())
        {
            LOG_ERROR("Asset not found %s", assetPath);
            return;
        }
        it->second.Decrement();
    }

    void DestroyAsset(const char* assetPath, AssetType type) { destroyAsset(assetPath, type); }
    void DestroyAsset(const std::string_view assetPath, AssetType type) { destroyAsset(assetPath, type); }

    template <typename T>
    static Ref<void> getAsset(T assetPath, AssetType type)
    {
        auto it = ir::HF.graphicsResources.assets.find(convertAssetPath(assetPath, type));
        if (it == ir::HF.graphicsResources.assets.end())
        {
            LOG_ERROR("Asset not found %s", assetPath);
            return nullptr;
        }
        return it->second.asset;
    }

    Ref<void> GetAsset(const char* assetPath, AssetType type) { return getAsset(assetPath, type); }
    Ref<void> GetAsset(const std::string_view assetPath, AssetType type) { return getAsset(assetPath, type); }

    namespace ir::rdr
    {
        void DestroyAllAssets_i(bool internalOnly)
        {
            for (auto& asset : std::ranges::views::values(HF.graphicsResources.assets)) asset.Destroy();
            if (!internalOnly) HF.graphicsResources.assets.clear();
        }
    }
}