#ifndef SHADERLAYOUTS_H
#define SHADERLAYOUTS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaderLayouts
    {
        hf::ShaderLayout default_lit{};
        hf::ShaderLayout default_unlit{};
    };

    extern AppShaderLayouts APP_SHADER_LAYOUTS;

    void ShaderLayoutDefineAll();
}

#endif //SHADERLAYOUTS_H
