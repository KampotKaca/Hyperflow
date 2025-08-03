#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderLayout Define(const ShaderLayoutDefinitionInfo& info)     { return inter::HF.renderingApi.api.DefineShaderLayout(info); }
    Buffer Define(const BufferDefinitionInfo& info)                 { return inter::HF.renderingApi.api.DefineUniformBuffer(info); }
    Buffer Define(const StorageBufferDefinitionInfo& info)          { return inter::HF.renderingApi.api.DefineStorageBuffer(info); }
    TextureSampler Define(const TextureSamplerDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureSampler(info); }
    TextureLayout Define(const TextureLayoutDefinitionInfo& info)   { return inter::HF.renderingApi.api.DefineTextureLayout(info); }
}