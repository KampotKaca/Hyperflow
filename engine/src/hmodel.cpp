#include "hmesh.h"
#include "hmodel.h"
#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hinternal.h"

#include "hyaml.h"
#include "hstrconversion.h"
#include "lz4.h"
#include "hmodelconvertor.h"

namespace hf
{
    static bool CheckFileIntegrity(const char* data, uint32_t& offset);
    static uint32_t ComputeVertexStride(MeshDataType dataType);
    static Ref<Mesh> LoadMesh(const ml::MeshHeader& header, const List<char>& meshData, uint32_t vertexStride, const Model* mesh, uint32_t& offset);

    Model::Model(const ModelCreationInfo& info) : filePath(info.filePath), meshStats(info.meshStats)
    {
        const auto modelLoc = TO_RES_PATH(std::string("models/") + filePath) + ".model";

        if (!utils::FileExists(modelLoc.c_str()))
        {
            LOG_ERROR("[Hyperflow] Unable to find model: %s", filePath.c_str());
            return;
        }

        List<char> compressedData{};
        if (!utils::ReadFile(modelLoc, compressedData))
        {
            LOG_ERROR("[Hyperflow] Unable to read model: %s", filePath.c_str());
            return;
        }

        uint32_t offset = 0;

        uint32_t meshCount = 0;
        memcpy(&meshCount, compressedData.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        List<ml::MeshHeader> headers(meshCount);
        memcpy(headers.data(), compressedData.data() + offset, meshCount * sizeof(ml::MeshHeader));
        offset += meshCount * sizeof(ml::MeshHeader);
        if (!CheckFileIntegrity(compressedData.data(), offset))
        {
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", filePath.c_str());
            return;
        }

        uint32_t meshSize = 0;
        for (auto& header : headers) meshSize += header.GetDataSize();

        List<char> meshData(meshSize);
        const auto maxSize = LZ4_decompress_safe(&compressedData[offset], meshData.data(), (int32_t)compressedData.size() - (int32_t)offset, (int32_t)meshSize);
        offset = 0;

        if (maxSize < 0)
        {
            LOG_ERROR("[Hyperflow] Unable to decompress mesh: %s", filePath.c_str());
            return;
        }

        const uint32_t vertexStride = ComputeVertexStride(meshStats.typeFlags);
        meshes.reserve(meshCount);

        for (uint32_t i = 0; i < meshCount; i++)
        {
            auto mesh = LoadMesh(headers[i], meshData, vertexStride, this, offset);
            meshes.push_back(mesh);
        }

        isLoaded = true;
    }

    Model::~Model()
    {
        ir::rdr::DestroyModel_i(this);
    }

    bool IsLoaded(const Ref<Model>& model) { return model->isLoaded; }

    uint32_t GetMeshCount(const Ref<Model>& model) { return (uint32_t)model->meshes.size(); }
    Ref<Mesh> GetMesh(const Ref<Model>& model, uint32_t index) { return model->meshes[index]; }

    namespace ir::rdr
    {
        Ref<Model> CreateModelAsset_i(const char* assetPath)
        {
            const std::string assetLoc = TO_RES_PATH(std::string("models/") + assetPath) + ".meta";
            List<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

            try
            {
                ModelCreationInfo info
                {
                    .filePath = assetPath,
                    .meshStats = {}
                };

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                auto typeFlags = root["typeFlags"];

                for (ryml::NodeRef fmt : typeFlags.children())
                {
                    const auto v = fmt.val();
                    const std::string_view valView{v.str, v.len};
                    info.meshStats.typeFlags |= STRING_TO_MESH_DATA_TYPE(valView);
                }

                {
                    const auto v = root["memoryType"].val();
                    const std::string_view memTypeView(v.str, v.len);
                    info.meshStats.memoryType = STRING_TO_BUFFER_MEMORY_TYPE(memTypeView);
                }

                {
                    const auto v = root["vertexAttribute"].val();
                    const std::string_view memTypeView(v.str, v.len);
                    info.meshStats.vertexAttribute = HF.graphicsResources.vertexAttributes[memTypeView];
                }

                return MakeRef<Model>(info);
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing BufferAttribute: %s", assetPath);
                return nullptr;
            }
        }

        bool DestroyModel_i(Model* model)
        {
            if (model->isLoaded)
            {
                model->isLoaded = false;
                for (auto& mesh : model->meshes) DestroyMesh_i(mesh.get());
                model->meshes.clear();
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

    Ref<Mesh> LoadMesh(
        const ml::MeshHeader& header,
        const List<char>& meshData, uint32_t vertexStride, const Model* model,
        uint32_t& offset)
    {
        List<VertexBufferCreationInfo> skinBuffers{};

        constexpr uint32_t pncStep = sizeof(float_t) * 3;
        constexpr uint32_t tStep = sizeof(float_t) * 2;

        auto* vertices = (unsigned char*)utils::Alloc(header.vertexCount * vertexStride);
        memset(vertices, 0, header.vertexCount * vertexStride);

#define INTEGRITY_CHECK\
        if (!CheckFileIntegrity(meshData.data(), offset))\
        {\
            utils::Deallocate(vertices);\
            LOG_ERROR("[Hyperflow] Corrupted mesh file, unable to load: %s", model->filePath.c_str());\
            return nullptr;\
        }

        uint32_t strideOffset = 0;

        if (((MeshDataType)header.dataFlags & MeshDataType::Position) != MeshDataType::None)
        {
            if ((model->meshStats.typeFlags & MeshDataType::Position) != MeshDataType::None)
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
        if ((model->meshStats.typeFlags & MeshDataType::Position) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::Normal) != MeshDataType::None)
        {
            if ((model->meshStats.typeFlags & MeshDataType::Normal) != MeshDataType::None)
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
        if ((model->meshStats.typeFlags & MeshDataType::Normal) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::Color) != MeshDataType::None)
        {
            if ((model->meshStats.typeFlags & MeshDataType::Color) != MeshDataType::None)
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
        if ((model->meshStats.typeFlags & MeshDataType::Color) != MeshDataType::None) strideOffset += pncStep;

        if (((MeshDataType)header.dataFlags & MeshDataType::TexCoord) != MeshDataType::None)
        {
            if ((model->meshStats.typeFlags & MeshDataType::TexCoord) != MeshDataType::None)
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

        //No point to load deformers if there are no positions in the vertices
        if (((MeshDataType)header.dataFlags & MeshDataType::Position) != MeshDataType::None)
        {
            skinBuffers.resize(header.skinDeformerCount);
            if (header.skinDeformerCount > 0)
            {
                constexpr uint32_t STRIDE = sizeof(uint32_t) * MAX_DEFORMATIONS_PER_VERTEX;

                for (uint32_t j = 0; j < header.skinDeformerCount; j++)
                {
                    uint32_t skinDataSize = header.vertexCount * STRIDE;
                    auto* skinVertices = (unsigned char*)utils::Alloc(skinDataSize);
                    memcpy(skinVertices, &meshData[offset], skinDataSize);

                    VertexBufferCreationInfo info{};
                    info.vertexSize = skinDataSize;
                    info.memoryType = BufferMemoryType::Static;
                    info.vertexCount = header.vertexCount;
                    info.pVertices = skinVertices;
                    info.usageFlags = BufferUsageTypeFlags::All;

                    skinBuffers[j] = info;
                    offset += skinDataSize;
                }

                INTEGRITY_CHECK
            }

            if (header.blendDeformerCount > 0)
            {
                constexpr uint32_t STRIDE = sizeof(float) * 4 * MAX_DEFORMATIONS_PER_VERTEX;

                for (uint32_t j = 0; j < header.blendDeformerCount; j++)
                {
                    uint32_t blendDataSize = header.vertexCount * STRIDE;
                    offset += blendDataSize;
                }

                INTEGRITY_CHECK
            }
        }

#undef INTEGRITY_CHECK

        uint32_t iSize = header.indexCount * BUFFER_DATA_SIZE[header.indexFormat];
        void* indices = utils::Alloc(iSize);
        memcpy(indices, &meshData[offset], iSize);
        offset += iSize;
        if (!CheckFileIntegrity(meshData.data(), offset))\
        {
            utils::Deallocate(vertices);
            utils::Deallocate(indices);
            LOG_ERROR("[Hyperflow] Corrupted model file, unable to load: %s", model->filePath.c_str());
            return nullptr;
        }

        VertexBufferCreationInfo vbInfo{};
        vbInfo.vertexSize = GetVertexSize(model->meshStats.vertexAttribute);
        vbInfo.memoryType = model->meshStats.memoryType;
        vbInfo.vertexCount = header.vertexCount;
        vbInfo.pVertices = vertices;

        IndexBufferCreationInfo ibInfo{};
        ibInfo.indexFormat = (BufferDataType)header.indexFormat;
        ibInfo.memoryType = model->meshStats.memoryType;
        ibInfo.indexCount = header.indexCount;
        ibInfo.pIndices = indices;

        MeshCreationInfo meshInfo{};
        meshInfo.vertexBufferCreationInfo = vbInfo;
        meshInfo.indexBufferCreationInfo = ibInfo;
        meshInfo.pSkinBufferCreationInfos = skinBuffers.data();
        meshInfo.skinBufferCount = (uint32_t)skinBuffers.size();
        meshInfo.stats = model->meshStats;
        meshInfo.volume = header.volume;

        return MakeRef<Mesh>(meshInfo);
    }
}
