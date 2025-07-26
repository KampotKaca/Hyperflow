#ifndef SHADERS_H
#define SHADERS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaderModules
    {
        uint32_t defaultVertexInput{};

        uint32_t default_lit_preRaster{};
        uint32_t default_unlit_preRaster{};

        uint32_t default_lit_fragment{};
        uint32_t default_unlit_fragment{};

        uint32_t defaultFragmentOutput{};
    };

    struct AppShaders
    {
        hf::Ref<hf::ShaderLibrary> library{};
        AppShaderModules modules{};

        hf::Ref<hf::Shader> default_lit{};
        hf::Ref<hf::Shader> default_unlit{};
    };

    extern AppShaders APP_SHADERS;
    void ShaderLoadAll();
}

#endif //SHADERS_H
