#ifndef HYPERFLOW_HMESHASHARED_H
#define HYPERFLOW_HMESHASHARED_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "../config.h"

namespace ml
{
#define MAX_DEFORMERS_PER_VERTEX 16

    struct SkinDeformer
    {
        uint8_t deformationCount{};
        float_t weights[MAX_DEFORMATIONS_PER_VERTEX]{};
        uint16_t bones[MAX_DEFORMATIONS_PER_VERTEX]{};

        bool operator==(const SkinDeformer& other) const = default;
    };

    struct BlendDeformer
    {
        uint8_t deformationCount{};
        glm::vec3 offsets[MAX_DEFORMATIONS_PER_VERTEX]{};

        bool operator==(const BlendDeformer& other) const = default;
    };

    struct Vertex
    {
        glm::vec3 pos{};
        glm::vec3 normal{};
        glm::vec3 color{};
        glm::vec2 texCoord{};

        SkinDeformer skinDeformers[MAX_DEFORMERS_PER_VERTEX]{};
        BlendDeformer blendDeformers[MAX_DEFORMERS_PER_VERTEX]{};

        bool operator==(const Vertex& other) const = default;
    };
}

namespace std
{
    template<> struct hash<ml::Vertex>
    {
        size_t operator()(ml::Vertex const& vertex) const noexcept
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                    (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                    (hash<glm::vec3>()(vertex.color) << 1) ^
                    (hash<glm::vec2>()(vertex.texCoord) >> 1);
        }
    };
}

#endif //HYPERFLOW_HMESHASHARED_H
