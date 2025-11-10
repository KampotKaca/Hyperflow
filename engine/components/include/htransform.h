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
            const float_t c3 = glm::cos(euler.z);
            const float_t s3 = glm::sin(euler.z);
            const float_t c2 = glm::cos(euler.x);
            const float_t s2 = glm::sin(euler.x);
            const float_t c1 = glm::cos(euler.y);
            const float_t s1 = glm::sin(euler.y);
            return mat4
            {
                vec4
                {
                    scale.x * (c1 * c3 + s1 * s2 * s3),
                    scale.x * (c2 * s3),
                    scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                vec4
                {
                    scale.y * (c3 * s1 * s2 - c1 * s3),
                    scale.y * (c2 * c3),
                    scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                vec4
                {
                    scale.z * (c2 * s1),
                    scale.z * (-s2),
                    scale.z * (c1 * c2),
                    0.0f,
                },
                vec4{ position.x, position.y, position.z, 1.0f }
            };
        }

        [[nodiscard]] mat3 ToNormalMat3() const
        {
            const float_t c3 = glm::cos(euler.z);
            const float_t s3 = glm::sin(euler.z);
            const float_t c2 = glm::cos(euler.x);
            const float_t s2 = glm::sin(euler.x);
            const float_t c1 = glm::cos(euler.y);
            const float_t s1 = glm::sin(euler.y);
            const vec3 invScale = 1.0f / scale;

            return mat3
            {
                vec3
                {
                    invScale.x * (c1 * c3 + s1 * s2 * s3),
                    invScale.x * (c2 * s3),
                    invScale.x * (c1 * s2 * s3 - c3 * s1),
                },
                vec3
                {
                    invScale.y * (c3 * s1 * s2 - c1 * s3),
                    invScale.y * (c2 * c3),
                    invScale.y * (c1 * c3 * s2 + s1 * s3),
                },
                vec3
                {
                    invScale.z * (c2 * s1),
                    invScale.z * (-s2),
                    invScale.z * (c1 * c2),
                },
            };
        }

        void GetNormalAndTransformMatrix(mat4* transform, mat3* normal) const
        {
            const float_t c3 = glm::cos(euler.z);
            const float_t s3 = glm::sin(euler.z);
            const float_t c2 = glm::cos(euler.x);
            const float_t s2 = glm::sin(euler.x);
            const float_t c1 = glm::cos(euler.y);
            const float_t s1 = glm::sin(euler.y);
            const vec3 invScale = 1.0f / scale;

            const auto v1 = vec3
            {
                (c1 * c3 + s1 * s2 * s3),
                (c2 * s3),
                (c1 * s2 * s3 - c3 * s1),
            };

            const auto v2 = vec3
            {
                (c3 * s1 * s2 - c1 * s3),
                (c2 * c3),
                (c1 * c3 * s2 + s1 * s3),
            };

            const auto v3 = vec3
            {
                (c2 * s1),
                (-s2),
                (c1 * c2),
            };

            *normal = mat3
            {
                invScale.x * v1,
                invScale.y * v2,
                invScale.z * v3
            };

            *transform = mat4
            {
                vec4(scale.x * v1, 0.0f),
                vec4(scale.y * v2, 0.0f),
                vec4(scale.z * v3, 0.0f),
                vec4{ position.x, position.y, position.z, 1.0f }
            };
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

        mutable mat4 trsMatrix{};
        mutable mat3 normalMatrix{};
        mutable BoundingVolume worldVolume{};

        void Update(const Transform& trs, const BoundingVolume& lVolume)
        {
            if (trs != transform || lVolume != localVolume)
            {
                transform = trs;
                localVolume = lVolume;
                trs.GetNormalAndTransformMatrix(&trsMatrix, &normalMatrix);

                worldVolume.min = vec3(std::numeric_limits<float>::max());
                worldVolume.max = vec3(std::numeric_limits<float>::lowest());

                const std::array corners =
                {
                    vec3(lVolume.min.x, lVolume.min.y, lVolume.min.z),
                    vec3(lVolume.max.x, lVolume.min.y, lVolume.min.z),
                    vec3(lVolume.min.x, lVolume.max.y, lVolume.min.z),
                    vec3(lVolume.max.x, lVolume.max.y, lVolume.min.z),
                    vec3(lVolume.min.x, lVolume.min.y, lVolume.max.z),
                    vec3(lVolume.max.x, lVolume.min.y, lVolume.max.z),
                    vec3(lVolume.min.x, lVolume.max.y, lVolume.max.z),
                    vec3(lVolume.max.x, lVolume.max.y, lVolume.max.z)
                };

                for (int i = 0; i < 8; i++)
                {
                    auto transformed = vec3(trsMatrix * vec4(corners[i], 1.0f));

                    worldVolume.min = glm::min(worldVolume.min, transformed);
                    worldVolume.max = glm::max(worldVolume.max, transformed);
                }
            }
        }
    };
}

#endif //HTRANSFORM_H
