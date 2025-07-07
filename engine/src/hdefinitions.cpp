#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineShaderSetup(info); }

    Buffer DefineUniformBuffer(const BufferDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineUniformBuffer(info); }
    Buffer DefineStorageBuffer(const StorageBufferDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineStorageBuffer(info); }
    BufferAllocator DefineBufferAllocator(const BufferAllocatorDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineBufferAllocator(info); }

    TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureSampler(info); }
    TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureLayout(info); }
}