#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hinternal.h"
#include "hmodelconvertor.h"
#include "hmesh.h"

namespace hf
{
    Mesh::Mesh(const MeshCreationInfo& info) : volume(info.volume), stats(info.stats)
    {
        vertBuffer = MakeRef<VertexBuffer>(info.vertexBufferCreationInfo, DataTransferType::TransferOwnership);
        indexBuffer = MakeRef<IndexBuffer>(info.indexBufferCreationInfo, DataTransferType::TransferOwnership);

        skinBuffers.resize(info.skinBufferCount);
        for (uint32_t j = 0; j < info.skinBufferCount; j++)
            skinBuffers[j] = MakeRef<VertexBuffer>(info.pSkinBufferCreationInfos[j], DataTransferType::TransferOwnership);
    }

    Mesh::~Mesh()
    {
        inter::rendering::DestroyMesh_i(this);
    }

    Ref<Mesh> Create(const MeshCreationInfo& info)
    {
        auto mesh = MakeRef<Mesh>(info);
        inter::HF.graphicsResources.meshes[(uint64_t)mesh.get()] = mesh;
        return mesh;
    }

    void Destroy(const Ref<Mesh>& mesh)
    {
        inter::rendering::DestroyMesh_i(mesh.get());
        inter::HF.graphicsResources.meshes.erase((uint64_t)mesh.get());
    }

    void Destroy(const Ref<Mesh>* pMeshes, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            inter::rendering::DestroyMesh_i(pMeshes[i].get());
            inter::HF.graphicsResources.meshes.erase((uint64_t)pMeshes[i].get());
        }
    }

    bool IsLoaded(const Ref<Mesh>& mesh) { return mesh->vertBuffer != nullptr; }

    MeshStats GetStats(const Ref<Mesh>& mesh) { return mesh->stats; }
    uint32_t GetMeshSkinCount(const Ref<Mesh>& mesh) { return mesh->skinBuffers.size(); }
    const BoundingVolume& GetMeshBoundingVolume(const Ref<Mesh>& mesh) { return mesh->volume; }

    namespace inter::rendering
    {
        bool DestroyMesh_i(Mesh* mesh)
        {
            if (mesh->vertBuffer)
            {
                DestroyBuffer_i(mesh->vertBuffer.get());
                DestroyBuffer_i(mesh->indexBuffer.get());

                for (auto& skin : mesh->skinBuffers) DestroyBuffer_i(skin.get());

                mesh->vertBuffer = nullptr;
                mesh->indexBuffer = nullptr;
                mesh->skinBuffers.clear();
                return true;
            }
            return false;
        }

        void DestroyAllMeshes_i(bool internalOnly)
        {
            for (const auto& mesh : std::ranges::views::values(HF.graphicsResources.meshes))
                DestroyMesh_i(mesh.get());
            if (!internalOnly) HF.graphicsResources.meshes.clear();
        }
    }
}
