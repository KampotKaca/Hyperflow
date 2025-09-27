#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hinternal.h"
#include "hmeshconvertor.h"
#include "hmesh.h"

#include "hmeshconvertor.h"
#include "hyaml.h"
#include "hstrconversion.h"
#include "lz4.h"

namespace hf
{
    static bool CheckFileIntegrity(const char* data, uint32_t& offset);
    static uint32_t ComputeVertexStride(MeshDataType dataType);
    static void LoadSubmeshUnique(const ml::SubMeshHeader& header, const std::vector<char>& meshData, uint32_t vertexStride, const Mesh* mesh, VertexBufferCreationInfo& vbInfo, IndexBufferCreationInfo& ibInfo, uint32_t& offset);

    Mesh::Mesh(const MeshCreationInfo& info) : filePath(info.filePath), stats(info.stats)
    {
        const auto meshLoc = TO_RES_PATH(std::string("meshes/") + filePath) + ".mesh";

        if (!utils::FileExists(meshLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find mesh: %s", filePath.c_str());
            return;
        }

        std::vector<char> compressedData{};
        if (!utils::ReadFile(meshLoc, compressedData))
        {
            LOG_ERROR("[Hyperflow] Unable to read mesh: %s", filePath.c_str());
            return;
        }

        uint32_t offset = 0;

        uint32_t submeshCount = 0;
        memcpy(&submeshCount, compressedData.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::vector<ml::SubMeshHeader> headers(submeshCount);
        memcpy(headers.data(), compressedData.data() + offset, submeshCount * sizeof(ml::SubMeshHeader));
        offset += submeshCount * sizeof(ml::SubMeshHeader);
        if (!CheckFileIntegrity(compressedData.data(), offset))
        {
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", filePath.c_str());
            return;
        }

        uint32_t meshSize = 0;
        for (auto& header : headers) meshSize += header.GetDataSize();

        std::vector<char> meshData(meshSize);
        const auto maxSize = LZ4_decompress_safe(&compressedData[offset], meshData.data(), (int32_t)compressedData.size() - (int32_t)offset, (int32_t)meshSize);
        offset = 0;

        if (maxSize < 0)
        {
            LOG_ERROR("[Hyperflow] Unable to decompress mesh: %s", filePath.c_str());
            return;
        }

        const uint32_t vertexStride = ComputeVertexStride(stats.typeFlags);
        subMeshes.reserve(submeshCount);

        for (uint32_t i = 0; i < submeshCount; i++)
        {
            auto& header = headers[i];

            VertexBufferCreationInfo vertInfo{};
            IndexBufferCreationInfo indexInfo{};

            LoadSubmeshUnique(header, meshData, vertexStride, this, vertInfo, indexInfo, offset);

            SubMesh submesh{};
            submesh.vertBuffer = MakeRef<VertexBuffer>(vertInfo, DataTransferType::TransferOwnership);
            submesh.indexBuffer = MakeRef<IndexBuffer>(indexInfo, DataTransferType::TransferOwnership);
            submesh.volume = header.volume;

            subMeshes.push_back(submesh);
        }

        isLoaded = true;
    }

    Mesh::~Mesh()
    {
        inter::rendering::DestroyMesh_i(this);
    }

    bool IsLoaded(const Ref<Mesh>& mesh) { return mesh->isLoaded; }
    MeshStats GetStats(const Ref<Mesh>& mesh) { return mesh->stats; }
    uint32_t GetSubmeshCount(const Ref<Mesh>& mesh) { return mesh->subMeshes.size(); }
    const BoundingVolume& GetSubmeshBoundingVolume(const Ref<Mesh>& mesh, uint32_t submeshIndex) { return mesh->subMeshes[submeshIndex].volume; }

    namespace inter::rendering
    {
        Ref<Mesh> CreateMeshAsset_i(const char* assetPath)
        {
            const std::string assetLoc = TO_RES_PATH(std::string("meshes/") + assetPath) + ".meta";
            std::vector<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

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
                    auto v = root["vertexAttribute"].val();
                    std::string_view memTypeView(v.str, v.len);
                    info.stats.vertexAttribute = HF.graphicsResources.vertexAttributes[memTypeView];
                }

                return MakeRef<Mesh>(info);
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing BufferAttribute: %s", assetPath);
                return nullptr;
            }
        }

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
                    DestroyBuffer_i(submesh.vertBuffer.get());
                    DestroyBuffer_i(submesh.indexBuffer.get());
                }

                mesh->isLoaded = false;
                return true;
            }
            return false;
        }
    }

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
        if ((dataType & MeshDataType::Position) != MeshDataType::None) vertexSize += 3 * sizeof(float_t);
        if ((dataType & MeshDataType::Normal)   != MeshDataType::None) vertexSize += 3 * sizeof(float_t);
        if ((dataType & MeshDataType::Color)    != MeshDataType::None) vertexSize += 3 * sizeof(float_t);
        if ((dataType & MeshDataType::TexCoord) != MeshDataType::None) vertexSize += 2 * sizeof(float_t);
        return vertexSize;
    }

    void LoadSubmeshUnique(const ml::SubMeshHeader& header, const std::vector<char>& meshData, uint32_t vertexStride, const Mesh* mesh, VertexBufferCreationInfo& vbInfo, IndexBufferCreationInfo& ibInfo, uint32_t& offset)
    {
        constexpr uint32_t pncStep = sizeof(float_t) * 3;
        constexpr uint32_t tStep = sizeof(float_t) * 2;

        auto* vertices = (unsigned char*)utils::Allocate(header.vertexCount * vertexStride);
        memset(vertices, 0, header.vertexCount * vertexStride);

#define INTEGRITY_CHECK\
        if (!CheckFileIntegrity(meshData.data(), offset))\
        {\
            utils::Deallocate(vertices);\
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", mesh->filePath.c_str());\
            return;\
        }

        uint32_t strideOffset = 0;

        if (((MeshDataType)header.dataFlags & MeshDataType::Position) != MeshDataType::None)
        {
            if ((mesh->stats.typeFlags & MeshDataType::Position) != MeshDataType::None)
            {
                for (uint32_t j = 0; j < header.vertexCount; j++)
                {
                    memcpy(&vertices[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                    // vec3 position = *(vec3*)&rf[j * vertexStride + strideOffset];
                    offset += pncStep;
                }
            }
            else offset += header.vertexCount * pncStep;
            INTEGRITY_CHECK
        }
        if ((mesh->stats.typeFlags & MeshDataType::Position) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::Normal) != MeshDataType::None)
        {
            if ((mesh->stats.typeFlags & MeshDataType::Normal) != MeshDataType::None)
            {
                for (uint32_t j = 0; j < header.vertexCount; j++)
                {
                    memcpy(&vertices[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                    offset += pncStep;
                }
            }
            else offset += header.vertexCount * pncStep;
            INTEGRITY_CHECK
        }
        if ((mesh->stats.typeFlags & MeshDataType::Normal) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::Color) != MeshDataType::None)
        {
            if ((mesh->stats.typeFlags & MeshDataType::Color) != MeshDataType::None)
            {
                for (uint32_t j = 0; j < header.vertexCount; j++)
                {
                    memcpy(&vertices[j * vertexStride + strideOffset], &meshData[offset], pncStep);
                    offset += pncStep;
                }
            }
            else offset += header.vertexCount * pncStep;
            INTEGRITY_CHECK
        }
        if ((mesh->stats.typeFlags & MeshDataType::Color) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::TexCoord) != MeshDataType::None)
        {
            if ((mesh->stats.typeFlags & MeshDataType::TexCoord) != MeshDataType::None)
            {
                for (uint32_t j = 0; j < header.vertexCount; j++)
                {
                    memcpy(&vertices[j * vertexStride + strideOffset], &meshData[offset], tStep);
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
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", mesh->filePath.c_str());
            return;
        }

        vbInfo.vertexSize = GetVertexSize(mesh->stats.vertexAttribute);
        vbInfo.memoryType = mesh->stats.memoryType;
        vbInfo.vertexCount = header.vertexCount;
        vbInfo.pVertices = vertices;

        ibInfo.indexFormat = (BufferDataType)header.indexFormat;
        ibInfo.memoryType = mesh->stats.memoryType;
        ibInfo.indexCount = header.indexCount;
        ibInfo.pIndices = indices;
    }
}
