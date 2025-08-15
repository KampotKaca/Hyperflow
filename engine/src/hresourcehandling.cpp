#include "hinternal.h"

namespace hf::inter
{
    void UnloadAllResources_i(const bool internalOnly)
    {
        rendering::DestroyAllBuffers_i(internalOnly);
        rendering::DestroyAllMeshes_i(internalOnly);
        rendering::DestroyAllShaders_i(internalOnly);
        rendering::DestroyAllShaderLibraries_i(internalOnly);

        rendering::DestroyAllTextures_i(internalOnly);
        rendering::DestroyAllCubemaps_i(internalOnly);

        rendering::DestroyAllTexturePacks_i(internalOnly);
        rendering::DestroyAllRenderTextures_i(internalOnly);
    }

    void CleanMarkedResources_i()
    {
        std::lock_guard lock(HF.deletedResources.syncLock);

        for (const auto shader : HF.deletedResources.shaders) HF.renderingApi.api.DestroyShader(shader);
        for (const auto lib : HF.deletedResources.shaderLibraries) HF.renderingApi.api.DestroyShaderLibrary(lib);
        for (auto buffer : HF.deletedResources.buffers)
        {
            switch (buffer.type)
            {
                case RuntimeBufferType::Vertex: HF.renderingApi.api.DestroyVertexBuffer(buffer.buffer); break;
                case RuntimeBufferType::Index: HF.renderingApi.api.DestroyIndexBuffer(buffer.buffer); break;
            }
        }
        for (const auto texPack : HF.deletedResources.texturePacks)
            HF.renderingApi.api.DestroyTexturePack(texPack);
        for (const auto tex : HF.deletedResources.textures)
            HF.renderingApi.api.DestroyTexture(tex);
        for (const auto tex : HF.deletedResources.renderTextures)
            HF.renderingApi.api.DestroyRenderTexture(tex);

        HF.deletedResources.shaderLibraries.clear();
        HF.deletedResources.shaders.clear();
        HF.deletedResources.buffers.clear();
        HF.deletedResources.texturePacks.clear();
        HF.deletedResources.textures.clear();
        HF.deletedResources.renderTextures.clear();
    }
}