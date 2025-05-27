#ifndef HCAMERA3DCORE_H
#define HCAMERA3DCORE_H

#include "hshared.h"

namespace hf
{
    struct Camera3DCore
    {
        float fov = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        [[nodiscard]] mat4 ToProjectionMat4(const Ref<Renderer>& rn) const;
    };
}

#endif //HCAMERA3DCORE_H
