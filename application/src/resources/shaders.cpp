#include "resources/shaders.h"

namespace app
{
    AppShaders APP_SHADERS;

    void ShaderLoadAll()
    {
        APP_SHADERS.library = hf::Cast<hf::ShaderLibrary>(hf::CreateAsset("app_shader_lib/app_shader_lib", hf::AssetType::ShaderLibrary));
        APP_SHADERS.default_lit = hf::Cast<hf::Shader>(hf::CreateAsset("default_lit", hf::AssetType::Shader));
        APP_SHADERS.default_unlit = hf::Cast<hf::Shader>(hf::CreateAsset("default_unlit", hf::AssetType::Shader));
    }
}