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

        bool IsVisible(const TransformedBoundingVolume& vol) const
        {
            if (!IsVisible(vol.sphereVolume)) return false;

            vec3 newMin( std::numeric_limits<float>::max() );
            vec3 newMax( std::numeric_limits<float>::lowest() );

            const std::array corners =
            {
                glm::vec3(vol.min.x, vol.min.y, vol.min.z),
                glm::vec3(vol.max.x, vol.min.y, vol.min.z),
                glm::vec3(vol.min.x, vol.max.y, vol.min.z),
                glm::vec3(vol.max.x, vol.max.y, vol.min.z),
                glm::vec3(vol.min.x, vol.min.y, vol.max.z),
                glm::vec3(vol.max.x, vol.min.y, vol.max.z),
                glm::vec3(vol.min.x, vol.max.y, vol.max.z),
                glm::vec3(vol.max.x, vol.max.y, vol.max.z)
            };

            for (int i = 0; i < 8; i++)
            {
                auto transformed = glm::vec3(vol.transform * glm::vec4(corners[i], 1.0f));

                newMin = glm::min(newMin, transformed);
                newMax = glm::max(newMax, transformed);
            }

            const BoundingVolume worldAABB{ newMin, newMax };
            return IsVisible(worldAABB);
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

        CameraUniformInfo GetUniformInfo(const Ref<Renderer>& rn) const;
        [[nodiscard]] mat4 ToViewProjectionMat4(const Ref<Renderer>& rn) const;
        void GetFrustum(const Ref<Renderer>& rn, CameraFrustum& result) const
        {
            ExtractFrustum(ToViewProjectionMat4(rn), result);
        }
    };
}

#endif //HCAMERA3DFREELOOK_H
