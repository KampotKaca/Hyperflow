#include "resources/shaders.h"
#include "resources/bufferattributes.h"
#include "resources/shadersetups.h"
#include "resources/renderpasses.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        //Default Shader
        {
            hf::ShaderCreationInfo shaderInfo
            {
                .renderPass = APP_RENDER_PASSES.mainPresentPass,
                .setup = APP_SHADER_SETUPS.viking_room,
                .supportedAttribCount = 1,
                .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                .vertexShaderLoc = "default",
                .fragmentShaderLoc = "default",
            };

            APP_SHADERS.viking_room = hf::Create(shaderInfo);
        }

        //Color shader
        {
            hf::ShaderCreationInfo shaderInfo
            {
                .renderPass = APP_RENDER_PASSES.mainPresentPass,
                .setup = APP_SHADER_SETUPS.color,
                .supportedAttribCount = 1,
                .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                .vertexShaderLoc = "color",
                .fragmentShaderLoc = "color",
            };

            APP_SHADERS.color = hf::Create(shaderInfo);
        }
    }
}