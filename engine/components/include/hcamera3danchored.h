#ifndef HCAMERA3DANCHORED_H
#define HCAMERA3DANCHORED_H

#include "hcamera3dcore.h"

namespace hf
{
    struct Camera3DAnchored
    {
        vec3 anchor{};
        vec3 up = { 0, 1, 0 };
        float_t yaw = 0.0f;
        float_t pitch = 0.0f;
        float_t distance = 10.0f;
        Camera3DCore core{};

        [[nodiscard]] vec3 GetDirection() const
        {
            const float_t yawRad = glm::radians(yaw);
            const float_t pitchRad = glm::radians(fmodf(pitch - 90, 180));
            const float_t cpr = cosf(pitchRad);
            return vec3{ cpr * sinf(yawRad), sinf(pitchRad), cpr * cosf(yawRad) };
        }

        [[nodiscard]] vec3 GetPosition() const
        {
            return anchor - GetDirection() * distance;
        }

        [[nodiscard]] mat4 ToViewMat4() const
        {
            return glm::lookAt(GetPosition(), anchor, -up);
        }

        [[nodiscard]] CameraUniformInfo GetUniformInfo(const Ref<Renderer>& rn) const;
        [[nodiscard]] mat4 ToViewProjectionMat4(const Ref<Renderer>& rn) const;
    };
}

#endif //HCAMERA3DANCHORED_H
