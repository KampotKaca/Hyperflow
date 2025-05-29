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
        hf::ShaderCreationInfo vikingShaderInfo
        {
            .renderPass = APP_RENDER_PASSES.mainPresentPass,
            .setup = APP_SHADER_SETUPS.viking_room,
            .texturePack = APP_TEXTURE_PACKS.viking_room_pack,
            .supportedAttribCount = 1,
            .pSupportedAttribs = &APP_BUFFER_ATTRIBUTES.pos_col_tex,
            .vertexShaderLoc = "default",
            .fragmentShaderLoc = "default",
        };

        APP_SHADERS.viking_room = hf::shader::Create(vikingShaderInfo);

        auto quadAttrib = hf::bufferattrib::GetQuad();
        hf::ShaderCreationInfo axisLinesShaderInfo
        {
            .renderPass = APP_RENDER_PASSES.mainPresentPass,
            .setup = APP_SHADER_SETUPS.axis_lines,
            .texturePack = nullptr,
            .supportedAttribCount = 1,
            .pSupportedAttribs = &quadAttrib,
            .vertexShaderLoc = "axislines",
            .fragmentShaderLoc = "axislines",
            .alphaTestOptions =
            {
                .blendMode = hf::ShaderBlendMode::Alpha,
                .blendOp = hf::ShaderBlendOp::XOr
            },
            .depthStencilOptions =
            {
                .enableDepth = false,
                .writeDepth = false,
                .comparisonFunc = hf::DepthComparisonFunction::Always,
                .enableDepthBounds = false,
                .enableStencil = false,
            }
        };
        APP_SHADERS.axis_lines = hf::shader::Create(axisLinesShaderInfo);
    }
}