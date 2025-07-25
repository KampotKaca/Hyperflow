#ifndef SHADERLAYOUTS_H
#define SHADERLAYOUTS_H

#include <hyperflow.h>

namespace app
{
    struct AppShaderLayouts
    {
        hf::ShaderLayout viking_room{};
        hf::ShaderLayout unlit_color{};
    };

    extern AppShaderLayouts APP_SHADER_LAYOUTS;

    void ShaderLayoutDefineAll();
}

#endif //SHADERLAYOUTS_H
