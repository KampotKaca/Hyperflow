#ifndef HSKYBOX_H
#define HSKYBOX_H

#include "hshared.h"

namespace hf
{
    struct Skybox
    {
        Skybox() = default;
        ~Skybox() = default;

        void Draw(const Ref<Renderer>& rn) const;
    };
}

#endif //HSKYBOX_H
