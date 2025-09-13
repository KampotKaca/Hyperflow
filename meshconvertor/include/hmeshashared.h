#ifndef HYPERFLOW_HMESHASHARED_H
#define HYPERFLOW_HMESHASHARED_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace ml
{
    struct Vertex
    {
        glm::vec3 pos{};
        glm::vec3 normal{};
        glm::vec3 color{};
        glm::vec2 texCoord{};

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos && color == other.color &&
                normal == other.normal && texCoord == other.texCoord;
        }
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
