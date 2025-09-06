#ifndef MESHCONVERTOR_H
#define MESHCONVERTOR_H

#include <cstring>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include "hshared.h"

struct SubMeshHeader
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t dataFlags = (uint32_t)hf::MeshDataType::None;
    uint32_t indexFormat = (uint32_t)hf::MeshIndexFormat::U16;
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
    std::vector<float> positions{};
    std::vector<float> normals{};
    std::vector<float> colors{};
    std::vector<float> texCoords{};
    std::vector<char> indices{};
};

struct MeshInfo
{
    std::vector<SubMeshHeader> headers{};
    std::vector<SubMeshInfo> subMeshes{};
};

#endif //MESHCONVERTOR_H
