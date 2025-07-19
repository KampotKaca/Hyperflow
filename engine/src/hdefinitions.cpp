#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderSetup Define(const ShaderSetupDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineShaderSetup(info); }

    Buffer Define(const BufferDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineUniformBuffer(info); }
    Buffer Define(const StorageBufferDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineStorageBuffer(info); }
    BufferAllocator Define(const BufferAllocatorDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineBufferAllocator(info); }

    TextureSampler Define(const TextureSamplerDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureSampler(info); }
    TextureLayout Define(const TextureLayoutDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureLayout(info); }
}