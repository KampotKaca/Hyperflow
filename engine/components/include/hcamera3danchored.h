#ifndef HCAMERA3DANCHORED_H
#define HCAMERA3DANCHORED_H

#include "hcamera3dcore.h"

namespace hf
{
    struct Camera3DAnchored
    {
        vec3 anchor{};
        vec3 up = { 0, 1, 0 };
        float yaw = 0.0f;
        float pitch = 0.0f;
        float distance = 10.0f;
        Camera3DCore core{};

        [[nodiscard]] inline vec3 GetDirection() const
        {
            float yawRad = glm::radians(yaw);
            float pitchRad = glm::radians(fmodf(pitch - 90, 180));
            float cpr = cosf(pitchRad);
            return { cpr * sinf(yawRad), sinf(pitchRad), cpr * cosf(yawRad) };
        }

        [[nodiscard]] inline mat4 ToViewMat4() const
        {
            vec3 direction = GetDirection();
            vec3 position{ anchor - direction * distance };
            return glm::lookAt(position, anchor, -up);
        }

        [[nodiscard]] mat4 ToViewProjectionMat4(const Ref<Renderer>& rn) const;
    };
}

#endif //HCAMERA3DANCHORED_H
