#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "hmodelconvertor.h"
#include "hmodelinternal.h"
#include "hstrconversion.h"
#include <lz4.h>
#include <fstream>

#include "hmodelshared.h"
#include "hyperflow.h"

namespace ml
{
    bool WriteMesh(const ModelInfo& meshInfo, const std::string& outputFilePath)
    {
        uint32_t submeshCount = meshInfo.headers.size();
        uint32_t headerDataSize = meshInfo.headers.size() * sizeof(MeshHeader);
        uint32_t fullHeaderSize = sizeof(uint32_t) + headerDataSize + 1;
        uint32_t offset = 0;
        auto headerData = hf::List<char>(fullHeaderSize);

        memcpy(headerData.data() + offset, &submeshCount, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(headerData.data() + offset, meshInfo.headers.data(), headerDataSize);
        offset += headerDataSize;
        headerData[offset] = '\0';

        offset = 0;
        hf::List<char> meshData{};
        for (uint32_t i = 0; i < submeshCount; i++)
        {
            auto& subMesh = meshInfo.subMeshes[i];
            offset = 0;

            //spacing is necessary to fast check the validity of the loaded mesh,
            //program just has \0 character for every data change and if after data copy it won't hit
            //this character this means something went wrong, this is not full proof solution but
            //is good enough for most cases that might happen.
            uint32_t spacingSize = 0;
            uint32_t positionSize = 0;
            uint32_t normalSize = 0;
            uint32_t colorSize = 0;
            uint32_t texcoordSize = 0;

            uint32_t boneWeightsSize = 0;
            uint32_t blendOffsetsSize = 0;

            uint32_t indexSize = subMesh.indices.size();

            if (!subMesh.positions.empty())
            {
                spacingSize++;
                positionSize = subMesh.positions.size() * sizeof(float_t);
            }
            if (!subMesh.normals.empty())
            {
                spacingSize++;
                normalSize = subMesh.normals.size() * sizeof(float_t);
            }
            if (!subMesh.colors.empty())
            {
                spacingSize++;
                colorSize = subMesh.colors.size() * sizeof(float_t);
            }
            if (!subMesh.texCoords.empty())
            {
                spacingSize++;
                texcoordSize = subMesh.texCoords.size() * sizeof(float_t);
            }
            if (!subMesh.boneWeights.empty())
            {
                spacingSize++;
                boneWeightsSize = subMesh.boneWeights.size() * sizeof(MeshInfo::BoneWeight);
            }
            if (!subMesh.blendOffsets.empty())
            {
                spacingSize++;
                blendOffsetsSize = subMesh.blendOffsets.size() * sizeof(float_t);
            }

            uint32_t submeshDataSize = positionSize + normalSize + colorSize + texcoordSize + indexSize +
                                       boneWeightsSize + blendOffsetsSize +
                                       spacingSize + 1;
            uint64_t oldSize = meshData.size();
            meshData.resize(meshData.size() + submeshDataSize);

#define STEP(x)\
            offset += x;\
            meshData[offset + oldSize] = '\0';\
            offset++

            if (positionSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.positions.data(), positionSize);
                STEP(positionSize);
            }

            if (normalSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.normals.data(), normalSize);
                STEP(normalSize);
            }

            if (colorSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.colors.data(), colorSize);
                STEP(colorSize);
            }

            if (texcoordSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.texCoords.data(), texcoordSize);
                STEP(texcoordSize);
            }

            if (boneWeightsSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.boneWeights.data(), boneWeightsSize);
                STEP(boneWeightsSize);
            }
            
            if (blendOffsetsSize > 0)
            {
                memcpy(meshData.data() + oldSize + offset, subMesh.blendOffsets.data(), blendOffsetsSize);
                STEP(blendOffsetsSize);
            }

            memcpy(meshData.data() + oldSize + offset, subMesh.indices.data(), indexSize);
            offset += indexSize;
            meshData[offset + oldSize] = '\0';
        }

        const int maxCompressedSize = LZ4_compressBound((int)meshData.size());
        auto compressedBuffer = (char*)hf::utils::Alloc(maxCompressedSize);
        hassert(compressedBuffer, "Failed to allocate memory: %s", outputFilePath.c_str());

        auto dataSize = LZ4_compress_default(meshData.data(), compressedBuffer, (int)meshData.size(), maxCompressedSize);
        hassert(dataSize > 0, "Failed to compress file: %s", outputFilePath.c_str());

        std::ofstream outFile(outputFilePath, std::ios::binary);
        hassert(outFile, "Failed to open output file: %s", outputFilePath.c_str());

        outFile.write(headerData.data(), fullHeaderSize);
        outFile.write(compressedBuffer, dataSize);
        outFile.close();

        hf::utils::Free(compressedBuffer);
        return true;
    }

    bool LoadModel(const char* path, ModelInfo* meshInfo)
    {
        const std::string_view sv(path);
        const auto dotPos = sv.find_last_of('.');
        hassert(dotPos != std::string_view::npos, "Unknown model type! cannot extract file extension!: %s", path);

        const std::string_view ext = sv.substr(dotPos + 1);
        const auto type = hf::STRING_TO_MODEL_TYPE(ext);

        bool modelLoadingResult = false;
        switch (type)
        {
        case hf::ModelType::Unknown: return false;
        case hf::ModelType::Obj:  modelLoadingResult = LoadObj(path, meshInfo); break;
        case hf::ModelType::Fbx:  modelLoadingResult = LoadFbx(path, meshInfo); break;
        case hf::ModelType::Gltf: modelLoadingResult = LoadGltf(path, meshInfo, true); break;
        case hf::ModelType::Glb:  modelLoadingResult = LoadGltf(path, meshInfo, false); break;
        }

        return modelLoadingResult;
    }

    void WriteData(ModelInfo* meshInfo, const hf::List<Vertex>& vertices, const hf::List<uint32_t>& indices,
        hf::MeshDataType dataFlags, uint16_t skinDeformationCount, uint16_t blendDeformationCount)
    {
        MeshInfo subMeshInfo{};

        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Position) subMeshInfo.positions.resize(vertices.size() * 3);
        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Normal)   subMeshInfo.normals.resize(vertices.size() * 3);
        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Color)    subMeshInfo.colors.resize(vertices.size() * 3);
        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::TexCoord) subMeshInfo.texCoords.resize(vertices.size() * 2);

        MeshHeader header{};
        header.vertexCount       = (uint32_t)vertices.size();
        header.indexCount        = (uint32_t)indices.size();
        header.dataFlags         = (uint32_t)dataFlags;
        header.skinDeformerCount  = skinDeformationCount;
        header.blendDeformerCount = blendDeformationCount;

        if (vertices.size() <= 65535)
        {
            header.indexFormat = (uint32_t)hf::MeshIndexFormat::U16;
            subMeshInfo.indices = hf::List<char>(indices.size() * 2);
            for (uint32_t i = 0; i < indices.size(); ++i)
            {
                auto result = (uint16_t)indices[i];
                memcpy(&subMeshInfo.indices[i * 2], &result, sizeof(uint16_t));
            }
        }
        else
        {
            header.indexFormat = (uint32_t)hf::MeshIndexFormat::U32;
            subMeshInfo.indices = hf::List<char>(indices.size() * 4);
            memcpy(subMeshInfo.indices.data(), indices.data(), sizeof(uint32_t) * indices.size());
        }

        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Position)
        {
            for (uint32_t i = 0; i < vertices.size(); ++i)
            {
                const auto& vertex = vertices[i];
                subMeshInfo.positions[i * 3 + 0] = vertex.pos.x;
                subMeshInfo.positions[i * 3 + 1] = vertex.pos.y;
                subMeshInfo.positions[i * 3 + 2] = vertex.pos.z;
            }

            if (vertices.size() > 0)
            {
                header.volume.min = vertices[0].pos;
                header.volume.max = header.volume.min;
                for (uint32_t i = 1; i < vertices.size(); ++i) header.volume.Encapsulate(vertices[i].pos);
            }
        }

        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Normal)
        {
            for (uint32_t i = 0; i < vertices.size(); ++i)
            {
                const auto& vertex = vertices[i];
                subMeshInfo.normals[i * 3 + 0] = vertex.normal.x;
                subMeshInfo.normals[i * 3 + 1] = vertex.normal.y;
                subMeshInfo.normals[i * 3 + 2] = vertex.normal.z;
            }
        }

        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::Color)
        {
            for (uint32_t i = 0; i < vertices.size(); ++i)
            {
                const auto& vertex = vertices[i];
                subMeshInfo.colors[i * 3 + 0] = vertex.color.x;
                subMeshInfo.colors[i * 3 + 1] = vertex.color.y;
                subMeshInfo.colors[i * 3 + 2] = vertex.color.z;
            }
        }

        if ((uint32_t)dataFlags & (uint32_t)hf::MeshDataType::TexCoord)
        {
            for (uint32_t i = 0; i < vertices.size(); ++i)
            {
                const auto& vertex = vertices[i];
                subMeshInfo.texCoords[i * 2 + 0] = vertex.texCoord.x;
                subMeshInfo.texCoords[i * 2 + 1] = vertex.texCoord.y;
            }
        }

        if (skinDeformationCount > 0)
        {
            subMeshInfo.boneWeights.resize(vertices.size() * skinDeformationCount * MAX_DEFORMATIONS_PER_VERTEX);

            for (size_t b = 0; b < vertices.size(); b++)
            {
                auto& vertex = vertices[b];
                size_t currentBoneWeight = 0;

                for (size_t c = 0; c < skinDeformationCount; c++)
                {
                    auto& deformer = vertex.skinDeformers[c];
                    for (size_t d = 0; d < deformer.deformationCount; d++)
                    {
                        auto bone = deformer.bones[d];
                        auto weight = deformer.weights[d];

                        MeshInfo::BoneWeight boneWeight{};
                        boneWeight.index = bone;
                        boneWeight.weight = MeshInfo::ToBoneWeight(weight);

                        subMeshInfo.boneWeights[(currentBoneWeight + d) * vertices.size() + b] = boneWeight;
                    }

                    currentBoneWeight += deformer.deformationCount;
                }
            }
        }

        if (blendDeformationCount > 0)
        {
            subMeshInfo.blendOffsets.resize(vertices.size() * blendDeformationCount * MAX_DEFORMATIONS_PER_VERTEX * 3);

            size_t currentBlendChannel = 0;
            for (size_t b = 0; b < vertices.size(); b++)
            {
                auto& vertex = vertices[b];

                for (size_t c = 0; c < blendDeformationCount; c++)
                {
                    auto& deformer = vertex.blendDeformers[c];
                    for (size_t d = 0; d < deformer.deformationCount; d++)
                    {
                        auto offset = deformer.offsets[d];
                        subMeshInfo.blendOffsets[(currentBlendChannel + d) * 3] = offset.x;
                        subMeshInfo.blendOffsets[(currentBlendChannel + d) * 3 + 1] = offset.y;
                        subMeshInfo.blendOffsets[(currentBlendChannel + d) * 3 + 2] = offset.z;
                    }

                    currentBlendChannel += deformer.deformationCount;
                }
            }
        }

        meshInfo->headers.push_back(header);
        meshInfo->subMeshes.push_back(std::move(subMeshInfo));
    }
}

int main(int argc, char* argv[])
{
    hassert(argc >= 3 && (argc - 1) % 2 == 0, "Usage: %s <input1> <output1> [<input2> <output2> ...]", argv[0])
    std::ios_base::sync_with_stdio(true);

    for (uint32_t i = 1; i < argc; i += 2)
    {
        std::string inputPath = argv[i];
        std::string outputPath = argv[i + 1];

        ml::ModelInfo meshInfo{};
        ml::LoadModel(inputPath.c_str(), &meshInfo);

        {
            namespace fs = std::filesystem;
            hassert(ml::WriteMesh(meshInfo, outputPath), "Error: Unable to export model %s", inputPath.c_str());
            log_info_s("Successfully converted %s -> %s", fs::path(inputPath).filename().string().c_str(), fs::path(outputPath).filename().string().c_str());
        }
    }

    return 0;
}