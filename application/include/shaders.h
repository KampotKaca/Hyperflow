#ifndef SHADERS_H
#define SHADERS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaders
    {
        hf::Ref<hf::Shader> viking_room_shader{};
        hf::ShaderSetup viking_room_setup{};
    };

    extern AppShaders APP_SHADERS;

    void ShaderDefineAll();
    void ShaderLoadAll();
}

#endif //SHADERS_H
