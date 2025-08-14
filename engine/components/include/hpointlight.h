#ifndef HPOINTLIGHT_H
#define HPOINTLIGHT_H

#include "hshared.h"

namespace hf
{
    struct PointLight
    {
        vec3 position { 0, 5, 0 };
        vec3 color { 1, 1, 1 };
        float_t range = 10;

        [[nodiscard]] LightUniformInfo::Point GetUniformInfo() const
        {
            return LightUniformInfo::Point
            {
                .position = position,
                .color = vec4(color, 1)
            };
        }
    };
}

#endif //HPOINTLIGHT_H
