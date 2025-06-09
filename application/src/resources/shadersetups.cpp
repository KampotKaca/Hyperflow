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
            std::array uniforms { hf::primitives::GetCameraUniform(), APP_UNIFORMS.timeUniform };
            const hf::ShaderSetupDefinitionInfo info
            {
                .pushConstant =
                {
                    .usageFlags = hf::ShaderUsageStage::Vertex | hf::ShaderUsageStage::Fragment,
                    .sizeInBytes = sizeof(hf::mat4)
                },
                .pBuffers = uniforms.data(),
                .bufferCount = uniforms.size(),
                .pTextureLayouts = &APP_TEXTURE_LAYOUTS.viking_room,
                .textureLayoutCount = 1
            };

            APP_SHADER_SETUPS.viking_room = hf::shadersetup::Define(info);
        }
    }
}