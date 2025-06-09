#include "hinternal.h"

namespace hf::inter
{
    void UnloadAllResources_i(const bool internalOnly)
    {
        rendering::DestroyAllVertBuffers_i(internalOnly);
        rendering::DestroyAllIndexBuffers_i(internalOnly);
        rendering::DestroyAllStorageBuffers_i(internalOnly);
        rendering::DestroyAllMeshes_i(internalOnly);
        rendering::DestroyAllShaders_i(internalOnly);

        rendering::DestroyAllTextures_i(internalOnly);
        rendering::DestroyAllCubemaps_i(internalOnly);

        rendering::DestroyAllTexturePackAllocators_i(internalOnly);
        rendering::DestroyAllTexturePacks_i(internalOnly);
    }

    void CleanMarkedResources_i()
    {
        std::lock_guard lock(HF.deletedResources.syncLock);

        for (auto shader : HF.deletedResources.shaders)
            HF.renderingApi.api.DestroyShader(shader);
        for (auto vBuffer : HF.deletedResources.vertBuffers)
            HF.renderingApi.api.DestroyVertBuffer(vBuffer);
        for (auto iBuffer : HF.deletedResources.indexBuffers)
            HF.renderingApi.api.DestroyIndexBuffer(iBuffer);
        for (auto sBuffer : HF.deletedResources.storageBuffers)
            HF.renderingApi.api.DestroyStorageBuffer(sBuffer);
        for (auto texPack : HF.deletedResources.texturePacks)
            HF.renderingApi.api.DestroyTexturePack(texPack);
        for (auto allocator : HF.deletedResources.texturePackAllocators)
            HF.renderingApi.api.DestroyTexturePackAllocator(allocator);
        for (auto tex : HF.deletedResources.textures)
            HF.renderingApi.api.DestroyTexture(tex);

        HF.deletedResources.shaders.clear();
        HF.deletedResources.vertBuffers.clear();
        HF.deletedResources.indexBuffers.clear();
        HF.deletedResources.storageBuffers.clear();
        HF.deletedResources.texturePacks.clear();
        HF.deletedResources.texturePackAllocators.clear();
        HF.deletedResources.textures.clear();
    }
}