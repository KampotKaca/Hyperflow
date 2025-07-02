#include "resources/uniforms.h"
#include "resources/texturelayouts.h"
#include "resources/shadersetups.h"
#include "resources/materials.h"

namespace app
{
    AppShaderSetups APP_SHADER_SETUPS;

    void ShaderSetupDefineAll()
    {
        //viking_room
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            const hf::ShaderSetupDefinitionInfo info
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

            APP_SHADER_SETUPS.viking_room = hf::DefineShaderSetup(info);
        }

        //color
        {
            std::array buffers { hf::primitives::GetGlobalUniformBuffer() };
            const hf::ShaderSetupDefinitionInfo info
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

            APP_SHADER_SETUPS.unlit_color = hf::DefineShaderSetup(info);
        }
    }
}