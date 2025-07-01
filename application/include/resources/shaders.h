#ifndef SHADERS_H
#define SHADERS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaders
    {
        hf::Ref<hf::Shader> viking_room{};
        hf::Ref<hf::Shader> color{};
    };

    extern AppShaders APP_SHADERS;
    void ShaderLoadAll();
}

#endif //SHADERS_H
