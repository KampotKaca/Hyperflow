#ifndef HCAMERA3DCORE_H
#define HCAMERA3DCORE_H

#include "hshared.h"

namespace hf
{
    struct CameraUniform
    {
        alignas(16) vec3 lookDirection;
        alignas(16) vec3 position;
        alignas(16) mat4 view{};
        alignas(16) mat4 invView{};
        alignas(16) mat4 proj{};
        alignas(16) mat4 invProj{};
        alignas(16) mat4 viewProj{};
    };

    struct Camera3DCore
    {
        float fov = 60.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        [[nodiscard]] mat4 ToProjectionMat4(const Ref<Renderer>& rn) const;
        static void BindCurrentToUniform(const Ref<Renderer>& rn);
    };
}

#endif //HCAMERA3DCORE_H
