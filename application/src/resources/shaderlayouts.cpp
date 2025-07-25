#include "resources/uniforms.h"
#include "resources/texturelayouts.h"
#include "resources/shaderlayouts.h"
#include "resources/materials.h"

namespace app
{
    AppShaderLayouts APP_SHADER_LAYOUTS;

    void ShaderLayoutDefineAll()
    {
        //viking_room
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            const hf::ShaderLayoutDefinitionInfo info
            {
                .pushConstant =
                {
                    .usageFlags = hf::ShaderUsageStage::Vertex | hf::ShaderUsageStage::Fragment,
                    .sizeInBytes = sizeof(DefaultPushConstant)
                },
                .pBuffers = buffers.data(),
                .bufferCount = buffers.size(),
                .pTextureLayouts = &APP_TEXTURE_LAYOUTS.viking_room,
                .textureLayoutCount = 1
            };

            APP_SHADER_LAYOUTS.viking_room = hf::Define(info);
        }

        //color
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            const hf::ShaderLayoutDefinitionInfo info
            {
                .pushConstant =
                {
                    .usageFlags = hf::ShaderUsageStage::Vertex | hf::ShaderUsageStage::Fragment,
                    .sizeInBytes = sizeof(UnlitColorPushConstant)
                },
                .pBuffers = buffers.data(),
                .bufferCount = buffers.size(),
                .pTextureLayouts = nullptr,
                .textureLayoutCount = 0
            };

            APP_SHADER_LAYOUTS.unlit_color = hf::Define(info);
        }
    }
}