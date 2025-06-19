#ifndef HCAMERA3DFREELOOK_H
#define HCAMERA3DFREELOOK_H

#include "hcamera3dcore.h"

namespace hf
{
    struct Camera3DFreeLook
    {
        vec3 position{};
        vec3 direction{};
        vec3 up = { 0, 1, 0 };
        Camera3DCore core{};

        [[nodiscard]] inline mat4 ToViewMat4() const
        {
            return glm::lookAt(position, position + direction, -up);
        }

        CameraUniformInfo GetUniformInfo(const Ref<Renderer>& rn) const;
        [[nodiscard]] mat4 ToViewProjectionMat4(const Ref<Renderer>& rn) const;
    };
}

#endif //HCAMERA3DFREELOOK_H
