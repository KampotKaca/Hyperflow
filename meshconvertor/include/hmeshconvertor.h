#ifndef MESHCONVERTOR_H
#define MESHCONVERTOR_H

#include <cstring>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include "hshared.h"

namespace ml
{
    struct SubMeshHeader
    {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        uint32_t dataFlags = (uint32_t)hf::MeshDataType::None;
        uint32_t indexFormat = (uint32_t)hf::MeshIndexFormat::U16;
        uint32_t skinDeformerCount = 0;
        uint32_t blendDeformerCount = 0;

        hf::BoundingVolume volume{};

        uint32_t GetDataSize() const
        {
            uint32_t dataSize = hf::BUFFER_DATA_SIZE[indexFormat] * indexCount + 1;
            if (dataFlags & (uint32_t)hf::MeshDataType::Position)
            {
                dataSize += vertexCount * 3 * sizeof(float);
                dataSize++;
            }
            if (dataFlags & (uint32_t)hf::MeshDataType::Normal)
            {
                dataSize += vertexCount * 3 * sizeof(float);
                dataSize++;
            }
            if (dataFlags & (uint32_t)hf::MeshDataType::Color)
            {
                dataSize += vertexCount * 3 * sizeof(float);
                dataSize++;
            }
            if (dataFlags & (uint32_t)hf::MeshDataType::TexCoord)
            {
                dataSize += vertexCount * 2 * sizeof(float);
                dataSize++;
            }
            return dataSize;
        }
    };

    struct SubMeshInfo
    {
        static constexpr float_t ToBoneWeight(uint16_t w) { return w / std::numeric_limits<uint16_t>().max(); }
        static constexpr uint16_t ToBoneWeight(float_t w) { return (uint16_t)(w * std::numeric_limits<uint16_t>().max()); }

        struct BoneWeight
        {
            uint16_t index{};
            uint16_t weight{};
        };

        std::vector<float> positions{};
        std::vector<float> normals{};
        std::vector<float> colors{};
        std::vector<float> texCoords{};
        std::vector<char> indices{};

        //deformations
        std::vector<uint8_t> boneCounts{};
        std::vector<BoneWeight> boneWeights{};

        std::vector<uint8_t> blendCounts{};
        std::vector<float> blendOffsets{};
    };

    struct MeshInfo
    {
        std::vector<SubMeshHeader> headers{};
        std::vector<SubMeshInfo> subMeshes{};
    };

    bool LoadModel(const char* path, MeshInfo* meshInfo);
    bool LoadObj(const char* path, MeshInfo* meshInfo);
    bool LoadFbx(const char* path, MeshInfo* meshInfo);
    bool LoadGltf(const char* path, MeshInfo* meshInfo, bool isGltf); //or Glb
}

#endif //MESHCONVERTOR_H
