#include "resources/shaders.h"
#include "resources/uniforms.h"
#include "resources/bufferattributes.h"
#include "resources/texturepacks.h"
#include "resources/shadersetups.h"
#include "resources/renderpasses.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        hf::ShaderCreationInfo shaderInfo
        {
            .renderPass = APP_RENDER_PASSES.mainPresentPass,
            .setup = APP_SHADER_SETUPS.viking_room_setup,
            .texturePack = APP_TEXTURE_PACKS.viking_room_pack,
            .supportedAttribCount = 1,
            .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pctAttribute,
            .vertexShaderLoc = "default",
            .fragmentShaderLoc = "default",
        };

        APP_SHADERS.viking_room_shader = hf::shader::Create(shaderInfo);
    }
}