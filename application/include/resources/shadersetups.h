#ifndef SHADERSETUPS_H
#define SHADERSETUPS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaderSetups
    {
        hf::ShaderSetup viking_room{};
        hf::ShaderSetup axis_lines{};
    };

    extern AppShaderSetups APP_SHADER_SETUPS;

    void ShaderSetupDefineAll();
}

#endif //SHADERSETUPS_H
