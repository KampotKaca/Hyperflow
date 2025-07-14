#ifndef HDIRECTIONALLIGHT_H
#define HDIRECTIONALLIGHT_H

#include "hexternal.h"

namespace hf
{
    struct DirectionalLight
    {
        vec3 euler { -90, 0, 0 };
        vec3 color { 1, 1, 1 };

        [[nodiscard]] LightUniformInfo::Directional GetUniformInfo() const
        {
            vec3 radians = glm::radians(euler);
            mat4 rot = glm::eulerAngleYXZ(radians.y, radians.x, radians.z);
            vec4 forward = rot * vec4(0, 0, -1, 0);
            return
            {
                .direction = vec3(forward),
                .color = color
            };
        }
    };
}

#endif //HDIRECTIONALLIGHT_H
