#ifndef SHADERS_H
#define SHADERS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaders
    {
        hf::Ref<hf::ShaderLibrary> library{};

        hf::Ref<hf::Shader> default_lit{};
        hf::Ref<hf::Shader> default_unlit{};
    };

    extern AppShaders APP_SHADERS;
    void ShaderLoadAll();
}

#endif //SHADERS_H
