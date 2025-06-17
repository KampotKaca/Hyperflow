#ifndef HMESH_H
#define HMESH_H

#include "hshared.h"

namespace hf
{
    struct Mesh
    {
        explicit Mesh(const MeshCreationInfo& info);
        ~Mesh();

        [[nodiscard]] bool IsRunning() const;
        [[nodiscard]] MeshStats GetStats() const;
        void Destroy();

        static Ref<Mesh> Create(const MeshCreationInfo& info);
        static Ref<Mesh> Create(const char* assetPath);
        static void Destroy(const Ref<Mesh>* pMeshes, uint32_t count);

#ifndef HF_ENGINE_INTERNALS
    private:
#endif
        struct SubMesh
        {
            Ref<VertBuffer> vertBuffer{};
            Ref<IndexBuffer> indexBuffer{};
        };

        std::string filePath{};
        bool isLoaded = false;
        MeshStats stats{};

        std::vector<SubMesh> subMeshes{};
    };
}

#endif //HMESH_H
