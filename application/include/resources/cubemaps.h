#ifndef CUBEMAPS_H
#define CUBEMAPS_H

#include "hyperflow.h"

namespace app
{
    struct AppCubemaps
    {
        hf::Ref<hf::Cubemap> skybox;
    };

    extern AppCubemaps APP_CUBEMAPS;

    void CubemapLoadAll();
}

#endif //CUBEMAPS_H
