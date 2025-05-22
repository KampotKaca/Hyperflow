#include "shaders.h"
#include "uniforms.h"
#include "textures.h"
#include "renderpasses.h"
#include "meshes.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderDefineAll()
    {
        hf::ShaderSetupDefinitionInfo shaderSetupDefinitionInfo
        {
            .pBuffers = &APP_UNIFORMS.cameraTimeBuffer,
            .bufferCount = 1,
            .pTextureLayouts = &APP_TEXTURES.viking_room_layout,
            .textureLayoutCount = 1
        };

        APP_SHADERS.viking_room_setup = hf::shadersetup::Define(shaderSetupDefinitionInfo);
    }

    void ShaderLoadAll()
    {
        hf::ShaderCreationInfo shaderInfo
        {
            .renderPass = APP_RENDER_PASSES.mainPresentPass,
            .setup = APP_SHADERS.viking_room_setup,
            .texturePack = APP_TEXTURES.viking_room_pack,
            .supportedAttribCount = 1,
            .pSupportedAttribs = &APP_MESHES.meshAttribute,
            .vertexShaderLoc = "default",
            .fragmentShaderLoc = "default",
        };

        APP_SHADERS.viking_room_shader = hf::shader::Create(shaderInfo);
    }
}