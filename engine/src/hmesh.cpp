#include "hmesh.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    Mesh::Mesh(const MeshCreationInfo& info) : filePath(info.filePath), stats(info.stats)
    {
        inter::rendering::CreateMeshBuffer_i(this);
    }

    Mesh::~Mesh()
    {
        inter::rendering::DestroyMeshBuffer_i(this);
    }

    namespace mesh
    {
        Ref<Mesh> Create(const MeshCreationInfo& info)
        {
            Ref<Mesh> mesh = MakeRef<Mesh>(info);
            inter::HF.graphicsResources.meshes[mesh.get()] = mesh;
            return mesh;
        }

        void Destroy(const Ref<Mesh>& mesh)
        {
            if (inter::rendering::DestroyMeshBuffer_i(mesh.get()))
                inter::HF.graphicsResources.meshes.erase(mesh.get());
        }

        void Destroy(const Ref<Mesh>* pMeshes, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto buffer = pMeshes[i];
                if (inter::rendering::DestroyMeshBuffer_i(buffer.get()))
                    inter::HF.graphicsResources.meshes.erase(buffer.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& mesh : std::ranges::views::values(inter::HF.graphicsResources.meshes))
                inter::rendering::DestroyMeshBuffer_i(mesh.get());
            if (!internalOnly) inter::HF.graphicsResources.meshes.clear();
        }

        bool IsRunning(const Ref<Mesh>& mesh) { return mesh->isLoaded; }
        MeshStats GetStats(const Ref<Mesh>& mesh) { return mesh->stats; }
    }

    namespace inter::rendering
    {
        bool CreateMeshBuffer_i(Mesh* mesh)
        {
            if (mesh->isLoaded) return false;

            mesh->isLoaded = true;
            return true;
        }
        bool DestroyMeshBuffer_i(Mesh* mesh)
        {
            if (mesh->isLoaded)
            {

                mesh->isLoaded = false;
                return true;
            }
            return false;
        }
    }
}
