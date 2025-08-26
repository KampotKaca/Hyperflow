#include "hasset.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
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
            case AssetType::Mesh:    inter::rendering::DestroyMesh_i(Cast<Mesh>(asset).get());           break;
            case AssetType::Texture: inter::rendering::DestroyTexture_i(Cast<Texture>(asset).get()); break;
            case AssetType::Cubemap: inter::rendering::DestroyCubemap_i(Cast<Cubemap>(asset).get());     break;
            default: LOG_ERROR("%s", "Invalid mesh type, cannot be destroyed"); break;
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
            case AssetType::Mesh:    asset = inter::rendering::CreateMeshAsset_i(assetPath);    break;
            case AssetType::Texture: asset = inter::rendering::CreateTextureAsset_i(assetPath); break;
            case AssetType::Cubemap: asset = inter::rendering::CreateCubemapAsset_i(assetPath); break;
            default: LOG_ERROR("%s", "Invalid mesh type, cannot be loaded"); break;
            }
        }
    }

    Ref<void> CreateAsset(const char* assetPath, AssetType type)
    {
        auto& assets = inter::HF.graphicsResources.assets;
        auto [it, inserted] = assets.try_emplace(assetPath);

        if (inserted) it->second.type = type;
        else if (it->second.type != type) LOG_ERROR("%s", "Asset type mismatch");

        it->second.Increment(assetPath);
        return it->second.asset;
    }

    namespace inter::rendering
    {
        void DestroyAllAssets_i(bool internalOnly)
        {
            for (auto& asset : std::ranges::views::values(HF.graphicsResources.assets)) asset.Destroy();
            if (!internalOnly) HF.graphicsResources.assets.clear();
        }
    }
}