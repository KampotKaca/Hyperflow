#ifndef HTRANSFORM_H
#define HTRANSFORM_H

#include "hshared.h"

namespace hf
{
    struct Transform
    {
        vec3 position = { 0, 0, 0 };
        vec3 euler = { 0, 0, 0 };
        vec3 scale = { 1, 1, 1 };

        [[nodiscard]] mat4 ToMat4() const
        {
            auto transform = mat4(1.0f);
            transform = glm::translate(transform, position)
                      * glm::toMat4(glm::quat(glm::radians(euler)))
                      * glm::scale(transform, scale);

            return transform;
        }
    };
}

#endif //HTRANSFORM_H
