#include "resources/shaders.h"
#include "resources/shaderlayouts.h"
#include "resources/rendertextures.h"
#include "../appconfig.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        //Shader Library!
        {
            //--------------- Vertex Input --------------------------------------------------
            std::array vertexInputModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryVertexInputModuleInfo> //Default
                {
                    .resultId = &APP_SHADERS.modules.defaultVertexInput,
                }
            };
            hf::utils::ReadVertexInputModule("default", vertexInputModules[0].module);

            //--------------- Pre Raster --------------------------------------------------

            std::array preRasterModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryPreRasterModuleInfo> //Default Lit
                {
                    .resultId = &APP_SHADERS.modules.default_lit_preRaster,
                },
                hf::ShaderLibraryModule<hf::ShaderLibraryPreRasterModuleInfo> //Default Unlit
                {
                    .resultId = &APP_SHADERS.modules.default_unlit_preRaster,
                },
            };

            hf::utils::ReadPreRasterModule("default_lit", APP_SHADER_LAYOUTS.default_lit, preRasterModules[0].module);
            hf::utils::ReadPreRasterModule("default_unlit", APP_SHADER_LAYOUTS.default_unlit, preRasterModules[1].module);

            //--------------- Fragment --------------------------------------------------

            std::array fragmentModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentModuleInfo> //Default Lit Fragment
                {
                    .resultId = &APP_SHADERS.modules.default_lit_fragment,
                },
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentModuleInfo> //Default Unlit Fragment
                {
                    .resultId = &APP_SHADERS.modules.default_unlit_fragment,
                },
            };

            hf::utils::ReadFragmentModule("default_lit", APP_SHADER_LAYOUTS.default_lit, fragmentModules[0].module);
            hf::utils::ReadFragmentModule("default_unlit", APP_SHADER_LAYOUTS.default_unlit, fragmentModules[1].module);

            //--------------- Fragment Output --------------------------------------------------

            std::array fragmentOutputModules
            {
                hf::ShaderLibraryModule<hf::ShaderLibraryFragmentOutputModuleInfo> //Default
                {
                    .resultId = &APP_SHADERS.modules.defaultFragmentOutput,
                },
            };
            hf::utils::ReadFragmentOutputModule("default", fragmentOutputModules[0].module);

            hf::ShaderDrawOutputFormats shaderOutputFormats{};
            shaderOutputFormats.sampleMode = MSAA_MODE;
            shaderOutputFormats.depthFormat = DEPTH_STENCIL_MODE;

            hf::ShaderLibraryCreationInfo info{};
            info.uniqueLibraryName         = "app_shader_lib_cache";
            info.outputFormats             = shaderOutputFormats;
            info.pVertexInputModules       = vertexInputModules.data();
            info.vertexInputModuleCount    = vertexInputModules.size();
            info.pPreRasterModules         = preRasterModules.data();
            info.preRasterModuleCount      = preRasterModules.size();
            info.pFragmentModules          = fragmentModules.data();
            info.fragmentModuleCount       = fragmentModules.size();
            info.pFragmentOutputModules    = fragmentOutputModules.data();
            info.fragmentOutputModuleCount = fragmentOutputModules.size();

            APP_SHADERS.library = Create(info);
        }

        //Default Lit Shader
        {
            hf::ShaderModulesInfo moduleInfo{};
            moduleInfo.vertexInputModuleId    = APP_SHADERS.modules.defaultVertexInput;
            moduleInfo.preRasterModuleId      = APP_SHADERS.modules.default_lit_preRaster;
            moduleInfo.fragmentModuleId       = APP_SHADERS.modules.default_lit_fragment;
            moduleInfo.fragmentOutputModuleId = APP_SHADERS.modules.defaultFragmentOutput;

            hf::ShaderCreationInfo shaderInfo{};
            shaderInfo.layout  = APP_SHADER_LAYOUTS.default_lit;
            shaderInfo.library = APP_SHADERS.library;
            shaderInfo.modules = moduleInfo;

            APP_SHADERS.default_lit = hf::Create(shaderInfo);
        }

        //Default Unlit shader
        {
            hf::ShaderModulesInfo moduleInfo{};
            moduleInfo.vertexInputModuleId    = APP_SHADERS.modules.defaultVertexInput;
            moduleInfo.preRasterModuleId      = APP_SHADERS.modules.default_unlit_preRaster;
            moduleInfo.fragmentModuleId       = APP_SHADERS.modules.default_unlit_fragment;
            moduleInfo.fragmentOutputModuleId = APP_SHADERS.modules.defaultFragmentOutput;

            hf::ShaderCreationInfo shaderInfo{};
            shaderInfo.layout = APP_SHADER_LAYOUTS.default_unlit;
            shaderInfo.library = APP_SHADERS.library;
            shaderInfo.modules = moduleInfo;

            APP_SHADERS.default_unlit = hf::Create(shaderInfo);
        }
    }
}