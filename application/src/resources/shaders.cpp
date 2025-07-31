#include "resources/shaders.h"
#include "resources/bufferattributes.h"
#include "resources/shaderlayouts.h"
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

        //Shader Library!
        {
            std::array vertexInputModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryVertexInputModuleInfo> //Default
                {
                    .resultId = &APP_SHADERS.modules.defaultVertexInput,
                    .module =
                    {
                        .attribute = APP_BUFFER_ATTRIBUTES.pos_nor_tex,
                    },
                }
            };

            std::array preRasterModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryPreRasterModuleInfo> //Default Lit Vertex Shader
                {
                    .resultId = &APP_SHADERS.modules.default_lit_preRaster,
                    .module =
                    {
                        .vertexShaderPath = { .path = "default_lit" },
                        .options = hf::ShaderRasterizerOptions
                        {
                            // .cullMode = hf::ShaderCullMode::None,
                            .polygonMode = hf::MeshPolygonMode::Fill,
                            .lineWidth = 1.0f
                        },
                        .layout = APP_SHADER_LAYOUTS.default_lit
                    }
                },
                hf::ShaderLibraryModule<hf::ShaderLibraryPreRasterModuleInfo> //Default Unlit Vertex Shader
                {
                    .resultId = &APP_SHADERS.modules.default_unlit_preRaster,
                    .module =
                    {
                        .vertexShaderPath = { .path = "default_unlit" },
                        .layout = APP_SHADER_LAYOUTS.default_unlit
                    }
                },
            };

            std::array fragmentModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentModuleInfo> //Default Lit Fragment
                {
                    .resultId = &APP_SHADERS.modules.default_lit_fragment,
                    .module =
                    {
                        .fragmentShaderPath = { .path = "default_lit" },
                        .layout = APP_SHADER_LAYOUTS.default_lit
                    }
                },
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentModuleInfo> //Default Unlit Fragment
                {
                    .resultId = &APP_SHADERS.modules.default_unlit_fragment,
                    .module =
                    {
                        .fragmentShaderPath = { .path = "default_unlit" },
                        .layout = APP_SHADER_LAYOUTS.default_unlit
                    }
                },
            };

            std::array fragmentOutputModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentOutputModuleInfo> //Default
                {
                    .resultId = &APP_SHADERS.modules.defaultFragmentOutput,
                },
            };

            const hf::ShaderLibraryCreationInfo info
            {
                .uniqueLibraryName = "app_shader_lib_cache",
                .outputFormats = shaderOutputFormats,
                .pVertexInputModules = vertexInputModules.data(),
                .vertexInputModuleCount = vertexInputModules.size(),
                .pPreRasterModules = preRasterModules.data(),
                .preRasterModuleCount = preRasterModules.size(),
                .pFragmentModules = fragmentModules.data(),
                .fragmentModuleCount = fragmentModules.size(),
                .pFragmentOutputModules = fragmentOutputModules.data(),
                .fragmentOutputModuleCount = fragmentOutputModules.size()
            };
            APP_SHADERS.library = Create(info);
        }

        //Default Lit Shader
        {
            const hf::ShaderCreationInfo shaderInfo
            {
                .layout = APP_SHADER_LAYOUTS.default_lit,
                .library = APP_SHADERS.library,
                .modules =
                {
                    .vertexInputModuleId = APP_SHADERS.modules.defaultVertexInput,
                    .preRasterModuleId = APP_SHADERS.modules.default_lit_preRaster,
                    .fragmentModuleId = APP_SHADERS.modules.default_lit_fragment,
                    .fragmentOutputModuleId = APP_SHADERS.modules.defaultFragmentOutput
                }
            };

            APP_SHADERS.default_lit = hf::Create(shaderInfo);
        }

        //Default Unlit shader
        {
            const hf::ShaderCreationInfo shaderInfo
            {
                .layout = APP_SHADER_LAYOUTS.default_unlit,
                .library = APP_SHADERS.library,
                .modules =
                {
                    .vertexInputModuleId = APP_SHADERS.modules.defaultVertexInput,
                    .preRasterModuleId = APP_SHADERS.modules.default_unlit_preRaster,
                    .fragmentModuleId = APP_SHADERS.modules.default_unlit_fragment,
                    .fragmentOutputModuleId = APP_SHADERS.modules.defaultFragmentOutput
                }
            };

            APP_SHADERS.default_unlit = hf::Create(shaderInfo);
        }
    }
}