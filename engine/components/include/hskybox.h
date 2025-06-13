#ifndef HSKYBOX_H
#define HSKYBOX_H

#include "hshared.h"

namespace hf
{
    struct Skybox
    {
        Skybox() = default;
        ~Skybox() = default;

        void BindCubemap(const Ref<Cubemap>& cubemap);
        void Draw(const Ref<Renderer>& rn);
    };
}

#endif //HSKYBOX_H
