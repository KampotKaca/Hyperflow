#include "resources/shaders.h"
#include "resources/shaderlayouts.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        APP_SHADERS.library = hf::Cast<hf::ShaderLibrary>(hf::CreateAsset("app_shader_lib/app_shader_lib", hf::AssetType::ShaderLibrary));

        //Default Lit Shader
        {
            hf::ShaderModulesInfo moduleInfo{};
            moduleInfo.vertexInputModuleId    = hf::GetVertexInputModule(APP_SHADERS.library, "default");
            moduleInfo.preRasterModuleId      = hf::GetPreRasterModule(APP_SHADERS.library, "default_lit");
            moduleInfo.fragmentModuleId       = hf::GetFragmentModule(APP_SHADERS.library, "default_lit");
            moduleInfo.fragmentOutputModuleId = hf::GetFragmentOutputModule(APP_SHADERS.library, "default");

            hf::ShaderCreationInfo shaderInfo{};
            shaderInfo.layout  = APP_SHADER_LAYOUTS.default_lit;
            shaderInfo.library = APP_SHADERS.library;
            shaderInfo.modules = moduleInfo;

            APP_SHADERS.default_lit = hf::Create(shaderInfo);
        }

        //Default Unlit shader
        {
            hf::ShaderModulesInfo moduleInfo{};
            moduleInfo.vertexInputModuleId    = hf::GetVertexInputModule(APP_SHADERS.library, "default");
            moduleInfo.preRasterModuleId      = hf::GetPreRasterModule(APP_SHADERS.library, "default_unlit");
            moduleInfo.fragmentModuleId       = hf::GetFragmentModule(APP_SHADERS.library, "default_unlit");
            moduleInfo.fragmentOutputModuleId = hf::GetFragmentOutputModule(APP_SHADERS.library, "default");

            hf::ShaderCreationInfo shaderInfo{};
            shaderInfo.layout = APP_SHADER_LAYOUTS.default_unlit;
            shaderInfo.library = APP_SHADERS.library;
            shaderInfo.modules = moduleInfo;

            APP_SHADERS.default_unlit = hf::Create(shaderInfo);
        }
    }
}