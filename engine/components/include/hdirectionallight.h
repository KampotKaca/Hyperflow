#ifndef HDIRECTIONALLIGHT_H
#define HDIRECTIONALLIGHT_H

#include "hshared.h"

namespace hf
{
    struct DirectionalLight
    {
        vec3 euler { -90, 0, 0 };
        vec3 color { 1, 1, 1 };
        float_t intensity = 1;

        [[nodiscard]] LightUniformInfo::Directional GetUniformInfo() const
        {
            const vec3 radians = glm::radians(euler);
            const mat4 rot = glm::eulerAngleYXZ(radians.y, radians.x, radians.z);
            const vec4 forward = rot * vec4(0, 0, -1, 0);
            return LightUniformInfo::Directional
            {
                .direction = forward,
                .color = vec4(color, intensity)
            };
        }
    };
}

#endif //HDIRECTIONALLIGHT_H
