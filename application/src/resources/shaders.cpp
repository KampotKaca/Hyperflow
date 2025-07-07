#include "resources/shaders.h"
#include "resources/bufferattributes.h"
#include "resources/shadersetups.h"
#include "resources/rendertextures.h"
#include "../appconfig.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        hf::ShaderDrawOutputFormats shaderOutputFormats
        {
            .sampleMode = MSAA_MODE,
            .depthFormat = DEPTH_STENCIL_MODE,
        };

        //Default Shader
        {
            hf::ShaderCreationInfo shaderInfo
            {
                .setup = APP_SHADER_SETUPS.viking_room,
                .supportedAttribCount = 1,
                .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                .vertexShaderLoc = "default",
                .fragmentShaderLoc = "default",
                .drawOutputFormats = shaderOutputFormats
            };

            APP_SHADERS.viking_room = hf::Create(shaderInfo);
        }

        //Color shader
        {
            hf::ShaderCreationInfo shaderInfo
            {
                .setup = APP_SHADER_SETUPS.unlit_color,
                .supportedAttribCount = 1,
                .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                .vertexShaderLoc = "unlit_color",
                .fragmentShaderLoc = "unlit_color",
                .drawOutputFormats = shaderOutputFormats
            };

            APP_SHADERS.unlit_color = hf::Create(shaderInfo);
        }
    }
}