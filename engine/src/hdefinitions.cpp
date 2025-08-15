#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderLayout Define(const ShaderLayoutDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineShaderLayout(info); }
    Buffer Define(const BufferDefinitionInfo& info)             { return inter::HF.renderingApi.api.DefineUniformBuffer(info); }
    Buffer Define(const StorageBufferDefinitionInfo& info)      { return inter::HF.renderingApi.api.DefineStorageBuffer(info); }

    VertexBufferAttribute FindVertexAttribute(const char* id) { return inter::HF.graphicsResources.vertexAttributes[id]; }
    TextureLayout FindTextureLayout(const char* id)           { return inter::HF.graphicsResources.textureLayouts[id]; }
    TextureSampler FindTextureSampler(const char* id)         { return inter::HF.graphicsResources.textureSamplers[id]; }
}