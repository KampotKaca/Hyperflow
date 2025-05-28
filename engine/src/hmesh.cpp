#include "hyperflow.h"
#include "hinternal.h"
#include "hmeshconvertor.h"
#include "hmesh.h"
#include "hyaml.h"
#include "hstrconversion.h"

namespace hf
{
    bool CheckFileIntegrity(const char* data, uint32_t& offset)
    {
        if (data[offset] == '\0')
        {
            offset++;
            return true;
        }
        return false;
    }

    uint32_t ComputeVertexStride(MeshDataType dataType)
    {
        uint32_t vertexSize = 0;
        if ((dataType & MeshDataType::Position) != MeshDataType::None) vertexSize += 3 * sizeof(float);
        if ((dataType & MeshDataType::Normal) != MeshDataType::None)   vertexSize += 3 * sizeof(float);
        if ((dataType & MeshDataType::Color) != MeshDataType::None)    vertexSize += 3 * sizeof(float);
        if ((dataType & MeshDataType::TexCoord) != MeshDataType::None) vertexSize += 2 * sizeof(float);
        return vertexSize;
    }

    Mesh::Mesh(const MeshCreationInfo& info) : filePath(info.filePath), stats(info.stats)
    {
        std::string meshLoc = TO_RES_PATH(std::string("meshes/") + filePath) + ".mesh";

        if (!utils::FileExists(meshLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find mesh: %s", filePath.c_str());
            return;
        }

        std::vector<char> meshData{};
        if (!utils::ReadFile(meshLoc, false, meshData))
        {
            LOG_ERROR("[Hyperflow] Unable to read mesh: %s", filePath.c_str());
            return;
        }
        uint32_t offset = 0;

        uint32_t submeshCount = 0;
        memcpy(&submeshCount, meshData.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::vector<SubMeshHeader> headers(submeshCount);
        memcpy(headers.data(), meshData.data() + offset, submeshCount * sizeof(SubMeshHeader));
        offset += submeshCount * sizeof(SubMeshHeader);

        if (!CheckFileIntegrity(meshData.data(), offset))
        {
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", filePath.c_str());
            return;
        }
        uint32_t vertexStride = ComputeVertexStride(stats.typeFlags);
        subMeshes.reserve(submeshCount);

        constexpr uint32_t pncStep = sizeof(float) * 3;
        constexpr uint32_t tStep = sizeof(float) * 2;

        for (uint32_t i = 0; i < submeshCount; i++)
        {
            auto& header = headers[i];
            void* vertices = utils::Allocate(header.vertexCount * vertexStride);
            auto* rf = (unsigned char*)vertices;
            memset(vertices, 0, header.vertexCount * vertexStride);

#define INTEGRITY_CHECK\
            if (!CheckFileIntegrity(meshData.data(), offset))\
            {\
                utils::Deallocate(vertices);\
                LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", filePath.c_str());\
                return;\
            }

            uint32_t strideOffset = 0;

            if (((MeshDataType)header.dataFlags & MeshDataType::Position) != MeshDataType::None)
            {
                if ((stats.typeFlags & MeshDataType::Position) != MeshDataType::None)
                {
                    for (uint32_t j = 0; j < header.vertexCount; j++)
                    {
                        memcpy(&rf[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                        offset += pncStep;
                    }
                }
                else offset += header.vertexCount * pncStep;
                INTEGRITY_CHECK
            }
            if ((stats.typeFlags & MeshDataType::Position) != MeshDataType::None) strideOffset += pncStep;

            if (((MeshDataType)header.dataFlags & MeshDataType::Normal) != MeshDataType::None)
            {
                if ((stats.typeFlags & MeshDataType::Normal) != MeshDataType::None)
                {
                    for (uint32_t j = 0; j < header.vertexCount; j++)
                    {
                        memcpy(&rf[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                        offset += pncStep;
                    }
                }
                else offset += header.vertexCount * pncStep;
                INTEGRITY_CHECK
            }
            if ((stats.typeFlags & MeshDataType::Normal) != MeshDataType::None) strideOffset += pncStep;

            if (((MeshDataType)header.dataFlags & MeshDataType::Color) != MeshDataType::None)
            {
                if ((stats.typeFlags & MeshDataType::Color) != MeshDataType::None)
                {
                    for (uint32_t j = 0; j < header.vertexCount; j++)
                    {
                        memcpy(&rf[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                        offset += pncStep;
                    }
                }
                else offset += header.vertexCount * pncStep;
                INTEGRITY_CHECK
            }
            if ((stats.typeFlags & MeshDataType::Color) != MeshDataType::None) strideOffset += pncStep;

            if (((MeshDataType)header.dataFlags & MeshDataType::TexCoord) != MeshDataType::None)
            {
                if ((stats.typeFlags & MeshDataType::TexCoord) != MeshDataType::None)
                {
                    for (uint32_t j = 0; j < header.vertexCount; j++)
                    {
                        memcpy(&rf[j * vertexStride + strideOffset], &meshData[offset], tStep);
                        offset += tStep;
                    }
                }
                else offset += header.vertexCount * tStep;
                INTEGRITY_CHECK
            }
#undef INTEGRITY_CHECK

            uint32_t iSize = header.indexCount * BUFFER_DATA_SIZE[header.indexFormat];
            void* indices = utils::Allocate(iSize);
            memcpy(indices, &meshData[offset], iSize);
            offset += iSize;
            if (!CheckFileIntegrity(meshData.data(), offset))\
            {
                utils::Deallocate(vertices);
                utils::Deallocate(indices);
                LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", filePath.c_str());
                return;
            }

            VertBufferCreationInfo vertInfo
            {
                .bufferAttrib = stats.bufferAttrib,
                .memoryType = stats.memoryType,
                .vertexCount = header.vertexCount,
                .pVertices = vertices
            };

            IndexBufferCreationInfo indexInfo
            {
                .indexFormat = (BufferDataType)header.indexFormat,
                .memoryType = stats.memoryType,
                .indexCount = header.indexCount,
                .pIndices = indices
            };

            SubMesh submesh
            {
                .vertBuffer = MakeRef<VertBuffer>(vertInfo, DataTransferType::TransferOwnership),
                .indexBuffer = MakeRef<IndexBuffer>(indexInfo, DataTransferType::TransferOwnership)
            };
            subMeshes.push_back(submesh);
        }

        isLoaded = true;
    }

    Mesh::~Mesh()
    {
        inter::rendering::DestroyMesh_i(this);
    }

    namespace mesh
    {
        Ref<Mesh> Create(const MeshCreationInfo& info)
        {
            Ref<Mesh> mesh = MakeRef<Mesh>(info);
            inter::HF.graphicsResources.meshes[mesh->filePath] = mesh;
            return mesh;
        }

        Ref<Mesh> Create(const char* assetPath)
        {
            std::string assetLoc = TO_RES_PATH(std::string("meshes/") + assetPath) + ".meta";
            if (!utils::FileExists(assetLoc.c_str()))
            {
                LOG_ERROR("[Hyperflow] Unable to find mesh meta file: %s", assetPath);
                return nullptr;
            }

            std::vector<char> metadata{};
            if (!utils::ReadFile(assetLoc, true, metadata))
            {
                LOG_ERROR("[Hyperflow] Unable to read mesh meta: %s", assetPath);
                return nullptr;
            }

            try
            {
                MeshCreationInfo info
                {
                    .filePath = assetPath,
                    .stats = {}
                };

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                auto typeFlags = root["typeFlags"];

                for (ryml::NodeRef fmt : typeFlags.children())
                {
                    auto v = fmt.val();
                    std::string_view valView{v.str, v.len};
                    info.stats.typeFlags |= STRING_TO_MESH_DATA_TYPE(valView);
                }

                {
                    auto v = root["memoryType"].val();
                    std::string_view memTypeView(v.str, v.len);
                    info.stats.memoryType = STRING_TO_BUFFER_MEMORY_TYPE(memTypeView);
                }

                {
                    auto v = root["bufferAttrib"].val();
                    std::string_view memTypeView(v.str, v.len);
                    info.stats.bufferAttrib = inter::HF.graphicsResources.bufferAttribs[memTypeView];
                }

                auto mesh = Create(info);
                inter::HF.graphicsResources.meshes[mesh->filePath] = mesh;
                return mesh;
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing BufferAttribute: %s", assetPath);
                return nullptr;
            }
        }

        void Destroy(const Ref<Mesh>& mesh)
        {
            if (inter::rendering::DestroyMesh_i(mesh.get()))
                inter::HF.graphicsResources.meshes.erase(mesh->filePath);
        }

        void Destroy(const Ref<Mesh>* pMeshes, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto mesh = pMeshes[i];
                if (inter::rendering::DestroyMesh_i(mesh.get()))
                    inter::HF.graphicsResources.meshes.erase(mesh->filePath);
            }
        }

        bool IsRunning(const Ref<Mesh>& mesh) { return mesh->isLoaded; }
        MeshStats GetStats(const Ref<Mesh>& mesh) { return mesh->stats; }
    }

    namespace inter::rendering
    {
        bool CreateMesh_i(Mesh* mesh)
        {
            if (mesh->isLoaded) return false;

            for (auto& submesh : mesh->subMeshes)
            {
                CreateVertBuffer_i(submesh.vertBuffer.get());
                CreateIndexBuffer_i(submesh.indexBuffer.get());
            }

            mesh->isLoaded = true;
            return true;
        }
        bool DestroyMesh_i(Mesh* mesh)
        {
            if (mesh->isLoaded)
            {
                for (auto& submesh : mesh->subMeshes)
                {
                    DestroyVertBuffer_i(submesh.vertBuffer.get());
                    DestroyIndexBuffer_i(submesh.indexBuffer.get());
                }

                mesh->isLoaded = false;
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

    namespace renderer
    {
        void Draw(const Ref<Renderer>& rn, const Ref<Mesh>& mesh)
        {
            for (auto& submesh : mesh->subMeshes)
            {
                DrawCallInfo drawInfo
                {
                    .pVertBuffers = &submesh.vertBuffer,
                    .bufferCount = 1,
                    .indexBuffer = submesh.indexBuffer,
                    .instanceCount = 1
                };
                Draw(rn, drawInfo);
            }
        }
    }
}
