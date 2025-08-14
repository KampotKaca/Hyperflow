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
            const hf::ShaderLayoutDefinitionInfo info
            {
                .pushConstant = hf::PushConstantInfo
                {
                    .usageFlags = hf::ShaderUsageStageFlags::Vertex | hf::ShaderUsageStageFlags::Fragment,
                    .sizeInBytes = sizeof(DefaultPushConstant)
                },
                .pBuffers = buffers.data(),
                .bufferCount = buffers.size(),
                .pTextureLayouts = &APP_TEXTURE_LAYOUTS.default_lit,
                .textureLayoutCount = 1
            };

            APP_SHADER_LAYOUTS.default_lit = hf::Define(info);
        }

        //Default Unlit
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            hf::ShaderLayoutDefinitionInfo info{};
            info.pBuffers = buffers.data();
            info.bufferCount = buffers.size();

            APP_SHADER_LAYOUTS.default_unlit = hf::Define(info);
        }
    }
}