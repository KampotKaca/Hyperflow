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

    inline bool operator==(const Transform& lhs, const Transform& rhs) noexcept
    {
        return lhs.position == rhs.position &&
               lhs.euler    == rhs.euler &&
               lhs.scale    == rhs.scale;
    }

    inline bool operator!=(const Transform& lhs, const Transform& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    struct VolumeTransform
    {
        Transform transform{};
        BoundingVolume localVolume{};

        mutable mat4 matrix{};
        mutable BoundingVolume worldVolume{};

        void Update(const Transform& trs, const BoundingVolume& lVolume)
        {
            if (trs != transform ||
                lVolume != localVolume)
            {
                transform = trs;
                localVolume = lVolume;
                matrix = trs.ToMat4();

                worldVolume.min = vec3(std::numeric_limits<float>::max());
                worldVolume.max = vec3(std::numeric_limits<float>::lowest());

                const std::array corners =
                {
                    glm::vec3(lVolume.min.x, lVolume.min.y, lVolume.min.z),
                    glm::vec3(lVolume.max.x, lVolume.min.y, lVolume.min.z),
                    glm::vec3(lVolume.min.x, lVolume.max.y, lVolume.min.z),
                    glm::vec3(lVolume.max.x, lVolume.max.y, lVolume.min.z),
                    glm::vec3(lVolume.min.x, lVolume.min.y, lVolume.max.z),
                    glm::vec3(lVolume.max.x, lVolume.min.y, lVolume.max.z),
                    glm::vec3(lVolume.min.x, lVolume.max.y, lVolume.max.z),
                    glm::vec3(lVolume.max.x, lVolume.max.y, lVolume.max.z)
                };

                for (int i = 0; i < 8; i++)
                {
                    auto transformed = glm::vec3(matrix * glm::vec4(corners[i], 1.0f));

                    worldVolume.min = glm::min(worldVolume.min, transformed);
                    worldVolume.max = glm::max(worldVolume.max, transformed);
                }
            }
        }
    };
}

#endif //HTRANSFORM_H
