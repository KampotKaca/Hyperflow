#ifndef HDIRECTIONALLIGHT_H
#define HDIRECTIONALLIGHT_H

#include "hexternal.h"

namespace hf
{
    struct DirectionalLight
    {
        vec3 direction { 0, -1, 0 };
        vec3 color { 1, 1, 1 };

        [[nodiscard]] LightUniformInfo::Directional GetUniformInfo() const
        {
            return
            {
                .direction = direction,
                .color = color
            };
        }
    };
}

#endif //HDIRECTIONALLIGHT_H
