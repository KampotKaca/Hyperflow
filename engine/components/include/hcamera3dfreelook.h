#ifndef HCAMERA3DFREELOOK_H
#define HCAMERA3DFREELOOK_H

#include "hcamera3dcore.h"
#include <immintrin.h>

namespace hf
{
    struct CameraFrustum
    {
        vec4 planes[6];

        bool IsVisible(const SphereVolume& volume) const
        {
            for (const auto& plane : planes)
            {
                const float dist = glm::dot(glm::vec3(plane), volume.center) + plane.w;
                if (dist < -volume.radius) return false;
            }
            return true;
        }

        bool IsVisible(const BoundingVolume& vol) const
        {
            for (const auto& plane : planes)
            {
                glm::vec3 positive = vol.min;
                if (plane.x >= 0) positive.x = vol.max.x;
                if (plane.y >= 0) positive.y = vol.max.y;
                if (plane.z >= 0) positive.z = vol.max.z;

                if (glm::dot(vec3(plane), positive) + plane.w < 0.0f) return false;
            }
            return true;
        }
    };

    inline void ExtractFrustum(const mat4& viewProj, CameraFrustum& result)
    {
        result.planes[0] = NormalizePlane(glm::row(viewProj, 3) + glm::row(viewProj, 0)); // Left
        result.planes[1] = NormalizePlane(glm::row(viewProj, 3) - glm::row(viewProj, 0)); // Right
        result.planes[2] = NormalizePlane(glm::row(viewProj, 3) + glm::row(viewProj, 1)); // Bottom
        result.planes[3] = NormalizePlane(glm::row(viewProj, 3) - glm::row(viewProj, 1)); // Top
        result.planes[4] = NormalizePlane(glm::row(viewProj, 3) + glm::row(viewProj, 2)); // Near
        result.planes[5] = NormalizePlane(glm::row(viewProj, 3) - glm::row(viewProj, 2)); // Far
    }

    struct Camera3DFreeLook
    {
        vec3 position{};
        vec3 direction{};
        vec3 up = { 0, 1, 0 };
        Camera3DCore core{};

        [[nodiscard]] mat4 ToViewMat4() const
        {
            return glm::lookAt(position, position + direction, -up);
        }

        CameraUniformInfo GetUniformInfo() const;
        [[nodiscard]] mat4 ToViewProjectionMat4() const;
        void GetFrustum(CameraFrustum& result) const
        {
            ExtractFrustum(ToViewProjectionMat4(), result);
        }
    };
}

#endif //HCAMERA3DFREELOOK_H
