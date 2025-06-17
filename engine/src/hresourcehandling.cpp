#include "hinternal.h"

namespace hf::inter
{
    void UnloadAllResources_i(const bool internalOnly)
    {
        rendering::DestroyAllBuffers_i(internalOnly);
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
        for (auto buffer : HF.deletedResources.buffers)
        {
            switch (buffer.type)
            {
                case BufferType::Vertex: HF.renderingApi.api.DestroyVertBuffer(buffer.buffer); break;
                case BufferType::Index: HF.renderingApi.api.DestroyIndexBuffer(buffer.buffer); break;
                case BufferType::Storage: HF.renderingApi.api.DestroyStorageBuffer(buffer.buffer); break;
            }
        }
        for (auto texPack : HF.deletedResources.texturePacks)
            HF.renderingApi.api.DestroyTexturePack(texPack);
        for (auto allocator : HF.deletedResources.texturePackAllocators)
            HF.renderingApi.api.DestroyTexturePackAllocator(allocator);
        for (auto tex : HF.deletedResources.textures)
            HF.renderingApi.api.DestroyTexture(tex);

        HF.deletedResources.shaders.clear();
        HF.deletedResources.buffers.clear();
        HF.deletedResources.texturePacks.clear();
        HF.deletedResources.texturePackAllocators.clear();
        HF.deletedResources.textures.clear();
    }
}