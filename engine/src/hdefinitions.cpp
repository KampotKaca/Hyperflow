#define HF_ENGINE_INTERNALS
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    RenderPass DefineRenderPass(const RenderPassDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineRenderPass(info); }
    ShaderSetup DefineShaderSetup(const ShaderSetupDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineShaderSetup(info); }

    UniformBuffer DefineUniformBuffer(const UniformBufferDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineUniformBuffer(info); }
    UniformAllocator DefineUniformAllocator(const UniformAllocatorDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineUniformAllocator(info); }

    TextureSampler DefineTextureSampler(const TextureSamplerDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureSampler(info); }
    TextureLayout DefineTextureLayout(const TextureLayoutDefinitionInfo& info) { return inter::HF.renderingApi.api.DefineTextureLayout(info); }
}