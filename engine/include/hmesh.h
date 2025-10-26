#ifndef HMESH_H
#define HMESH_H

#include "hshared.h"

namespace hf
{
    struct Mesh
    {
        explicit Mesh(const MeshCreationInfo& info);
        ~Mesh();

        std::string name{};

        Ref<VertexBuffer> vertBuffer{};
        Ref<IndexBuffer> indexBuffer{};
        BoundingVolume volume{};
        uint32_t vertexStride{};

        List<Ref<VertexBuffer>> skinBuffers{};

        MeshStats stats{};
    };
}

#endif //HMESH_H
