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
