#include "resources/uniforms.h"
#include "resources/texturelayouts.h"
#include "resources/shadersetups.h"

namespace app
{
    AppShaderSetups APP_SHADER_SETUPS;

    void ShaderSetupDefineAll()
    {
        //viking_room
        {
            const hf::ShaderSetupDefinitionInfo info
            {
                .pushConstant =
                {
                    .usageFlags = hf::ShaderUsageStage::Vertex | hf::ShaderUsageStage::Fragment,
                    .sizeInBytes = sizeof(hf::mat4)
                },
                .pBuffers = APP_UNIFORMS.uniforms.data(),
                .bufferCount = 2,
                .pTextureLayouts = &APP_TEXTURE_LAYOUTS.viking_room,
                .textureLayoutCount = 1
            };

            APP_SHADER_SETUPS.viking_room = hf::shadersetup::Define(info);
        }

        //axis_lines
        {
            auto emptyLayout = hf::resources::GetEmptyTextureLayout();
            const hf::ShaderSetupDefinitionInfo info
            {
                .pushConstant =
                {
                    .usageFlags = hf::ShaderUsageStage::Vertex | hf::ShaderUsageStage::Fragment,
                    .sizeInBytes = sizeof(hf::AxisLines::UploadInfo),
                },
                .pBuffers = &APP_UNIFORMS.cameraUniform,
                .bufferCount = 1,
                .pTextureLayouts = &emptyLayout,
                .textureLayoutCount = 1
            };

            APP_SHADER_SETUPS.axis_lines = hf::shadersetup::Define(info);
        }
    }
}