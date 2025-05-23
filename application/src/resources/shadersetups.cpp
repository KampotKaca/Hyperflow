#include "resources/uniforms.h"
#include "resources/texturelayouts.h"
#include "resources/shadersetups.h"

namespace app
{
    AppShaderSetups APP_SHADER_SETUPS;

    void ShaderSetupDefineAll()
    {
        hf::ShaderSetupDefinitionInfo shaderSetupDefinitionInfo
        {
            .pBuffers = &APP_UNIFORMS.cameraTimeBuffer,
            .bufferCount = 1,
            .pTextureLayouts = &APP_TEXTURE_LAYOUTS.viking_room_layout,
            .textureLayoutCount = 1
        };

        APP_SHADER_SETUPS.viking_room_setup = hf::shadersetup::Define(shaderSetupDefinitionInfo);
    }
}