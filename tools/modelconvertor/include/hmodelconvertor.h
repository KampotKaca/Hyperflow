#ifndef MESHCONVERTOR_H
#define MESHCONVERTOR_H

#include <filesystem>
#include <vector>
#include "henums.h"
#include "hmath.h"

namespace ml
{
    struct MeshHeader
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

    struct MeshInfo
    {
        static constexpr float_t ToBoneWeight(uint16_t w) { return w / std::numeric_limits<uint16_t>().max(); }
        static constexpr uint16_t ToBoneWeight(float_t w) { return (uint16_t)(w * std::numeric_limits<uint16_t>().max()); }

        struct BoneWeight
        {
            uint16_t index{};
            uint16_t weight{};
        };

        hf::List<float> positions{};
        hf::List<float> normals{};
        hf::List<float> colors{};
        hf::List<float> texCoords{};
        hf::List<char> indices{};

        //deformations
        hf::List<BoneWeight> boneWeights{};
        hf::List<float> blendOffsets{};
    };

    struct ModelInfo
    {
        hf::List<MeshHeader> headers{};
        hf::List<MeshInfo> subMeshes{};
    };

    bool LoadModel(const char* path, ModelInfo* meshInfo);
    bool LoadObj(const char* path, ModelInfo* meshInfo);
    bool LoadFbx(const char* path, ModelInfo* meshInfo);
    bool LoadGltf(const char* path, ModelInfo* meshInfo, bool isGltf); //or Glb
}

#endif //MESHCONVERTOR_H
