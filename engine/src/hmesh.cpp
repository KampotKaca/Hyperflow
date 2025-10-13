#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hinternal.h"
#include "hmodelconvertor.h"
#include "hmesh.h"

#include "hyaml.h"
#include "lz4.h"

namespace hf
{
    Mesh::Mesh(const MeshCreationInfo& info) :
    vertCreationInfo(info.vertexBufferCreationInfo),
    indexCreationInfo(info.indexBufferCreationInfo),
    volume(info.volume), stats(info.stats)
    {
        skinBufferCreationInfos = std::vector<VertexBufferCreationInfo>(info.skinBufferCount);
        memcpy(skinBufferCreationInfos.data(), info.pSkinBufferCreationInfos, sizeof(VertexBufferCreationInfo) * info.skinBufferCount);

        inter::rendering::CreateMesh_i(this);
    }

    Mesh::~Mesh()
    {
        inter::rendering::DestroyMesh_i(this);
    }

    MeshStats GetStats(const Ref<Mesh>& mesh) { return mesh->stats; }
    uint32_t GetMeshSkinCount(const Ref<Mesh>& mesh) { return mesh->skinBuffers.size(); }
    const BoundingVolume& GetMeshBoundingVolume(const Ref<Mesh>& mesh) { return mesh->volume; }

    namespace inter::rendering
    {
        bool CreateMesh_i(Mesh* mesh)
        {
            if (mesh->vertBuffer) return false;

            mesh->vertBuffer = MakeRef<VertexBuffer>(mesh->vertCreationInfo, DataTransferType::TransferOwnership);
            mesh->indexBuffer = MakeRef<IndexBuffer>(mesh->indexCreationInfo, DataTransferType::TransferOwnership);

            mesh->skinBuffers.resize(mesh->skinBufferCreationInfos.size());
            for (uint32_t j = 0; j < mesh->skinBufferCreationInfos.size(); j++)
                mesh->skinBuffers[j] = MakeRef<VertexBuffer>(mesh->skinBufferCreationInfos[j], DataTransferType::TransferOwnership);

            return true;
        }

        bool DestroyMesh_i(Mesh* mesh)
        {
            if (!mesh->vertBuffer) return false;

            DestroyBuffer_i(mesh->vertBuffer.get());
            DestroyBuffer_i(mesh->indexBuffer.get());

            for (auto& skin : mesh->skinBuffers) DestroyBuffer_i(skin.get());

            mesh->vertBuffer = nullptr;
            mesh->indexBuffer = nullptr;
            mesh->skinBuffers.clear();

            return true;
        }
    }
}
