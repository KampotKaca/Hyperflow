#ifndef HCAMERA3DFREELOOK_H
#define HCAMERA3DFREELOOK_H

#include "hcamera3dcore.h"

namespace hf
{
    struct CameraFrustum
    {
        vec4 planes[6];

        bool IsVisible(const SphereVolume& volume)
        {
            for (const auto& plane : planes)
            {
                const float dist = glm::dot(glm::vec3(plane), volume.center) + plane.w;
                if (dist < -volume.radius) return false;
            }
            return true;
        }

        template<typename T, bool ApplyTransform>
        bool IsVisible(const T& vol)
        {
            if constexpr (ApplyTransform)
            {
                if (!IsVisible(vol.sphereVolume)) return false;
            }

            std::array corners =
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

            if constexpr (ApplyTransform)
            {
                for (auto& c : corners)
                    c = glm::vec3(vol.transform * glm::vec4(c, 1.0f));
            }

            for (const auto& plane : planes)
            {
                int insideCount = 0;
                for (const auto& c : corners)
                {
                    if (glm::dot(glm::vec3(plane), c) + plane.w >= 0.0f)
                    {
                        insideCount++;
                        break;
                    }
                }

                if (insideCount == 0) return false;
            }

            return true;
        }

        bool IsVisible(const TransformedBoundingVolume& vol) { return IsVisible<TransformedBoundingVolume, true>(vol); }
        bool IsVisible(const BoundingVolume& vol)            { return IsVisible<BoundingVolume, false>(vol); }
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
