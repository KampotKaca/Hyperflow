#ifndef HMESH_H
#define HMESH_H

#include "hshared.h"

namespace hf
{
    struct Mesh
    {
        explicit Mesh(const MeshCreationInfo& info);
        ~Mesh();

        struct SubMesh
        {
            Ref<VertexBuffer> vertBuffer{};
            Ref<IndexBuffer> indexBuffer{};
            BoundingVolume volume{};
        };

        std::string filePath{};
        bool isLoaded = false;
        MeshStats stats{};

        std::vector<SubMesh> subMeshes{};
    };
}

#endif //HMESH_H
