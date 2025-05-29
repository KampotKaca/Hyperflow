#include "resources/uniforms.h"
#include "resources/texturelayouts.h"
#include "resources/shadersetups.h"

namespace app
{
    AppShaderSetups APP_SHADER_SETUPS;

    void ShaderSetupDefineAll()
    {
        hf::ShaderSetupDefinitionInfo vikingRoomDefinitionInfo
        {
            .pBuffers = &APP_UNIFORMS.cameraTimeBuffer,
            .bufferCount = 1,
            .pTextureLayouts = &APP_TEXTURE_LAYOUTS.viking_room,
            .textureLayoutCount = 1
        };

        APP_SHADER_SETUPS.viking_room = hf::shadersetup::Define(vikingRoomDefinitionInfo);

        std::array<hf::UniformBuffer, 2> axisLinesUniforms = { APP_UNIFORMS.cameraTimeBuffer };
        hf::AxisLines::GetNecessaryUniforms(&axisLinesUniforms[1]);

        auto emptyLayout = hf::texturelayout::GetEmpty();
        hf::ShaderSetupDefinitionInfo axisLinesDefinitionInfo
        {
            .pBuffers = axisLinesUniforms.data(),
            .bufferCount = axisLinesUniforms.size(),
            .pTextureLayouts = &emptyLayout,
            .textureLayoutCount = 1
        };

        APP_SHADER_SETUPS.axis_lines = hf::shadersetup::Define(axisLinesDefinitionInfo);
    }
}