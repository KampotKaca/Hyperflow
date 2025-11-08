#ifndef HCAMERA3DCORE_H
#define HCAMERA3DCORE_H

#include "hshared.h"

namespace hf
{
    struct Camera3DCore
    {
        float_t fov = 60.0f;
        float_t nearPlane = 0.1f;
        float_t farPlane = 1000.0f;

        void SetFov(const float_t newValue) { fov = glm::clamp(newValue, 1.0f, 179.0f); }
        [[nodiscard]] mat4 ToProjectionMat4() const;
    };
}

#endif //HCAMERA3DCORE_H
