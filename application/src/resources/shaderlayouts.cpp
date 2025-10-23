#include "resources/texturelayouts.h"
#include "resources/shaderlayouts.h"
#include "resources/materials.h"

namespace app
{
    AppShaderLayouts APP_SHADER_LAYOUTS;

    void ShaderLayoutDefineAll()
    {
        //Default Lit
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            hf::ShaderLayoutDefinitionInfo info{};
            info.pushConstant.usageFlags = hf::ShaderUsageStageFlags::Vertex | hf::ShaderUsageStageFlags::Fragment;
            info.pushConstant.sizeInBytes = sizeof(DefaultPushConstant);
            info.pBuffers = buffers.data();
            info.bufferCount = buffers.size();
            info.pTextureLayouts = &APP_TEXTURE_LAYOUTS.default_lit;
            info.textureLayoutCount = 1;

            APP_SHADER_LAYOUTS.default_lit = hf::Define("default_lit", info);
        }

        //Default Unlit
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            hf::ShaderLayoutDefinitionInfo info{};
            info.pBuffers = buffers.data();
            info.bufferCount = buffers.size();

            APP_SHADER_LAYOUTS.default_unlit = hf::Define("default_unlit", info);
        }
    }
}