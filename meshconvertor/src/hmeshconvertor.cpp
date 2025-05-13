#define GLM_ENABLE_EXPERIMENTAL
#include <cstring>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include <glm/gtx/hash.hpp>

#define DEFINE_ENUM_FLAGS(Enum)\
inline Enum operator|(Enum a, Enum b) { return (Enum)((uint32_t)a | (uint32_t)b); }\
inline Enum operator&(Enum a, Enum b) { return (Enum)((uint32_t)a & (uint32_t)b); }\
inline Enum& operator|=(Enum& a, Enum b)\
{\
a = a | b;\
return a;\
}\
inline Enum& operator&=(Enum& a, Enum b)\
{\
a = a & b;\
return a;\
}\

struct Vertex
{
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec3 color{};
    glm::vec2 texCoord{};

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color &&
            normal == other.normal && texCoord == other.texCoord;
    }
};

namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const noexcept
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                   (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
                   (hash<glm::vec3>()(vertex.color) << 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) >> 1);
        }
    };
}

enum class SubmeshDataType
{
    None = 0,
    Position = 1 << 0, Normal = 1 << 1, Color = 1 << 2,
    TexCoord = 1 << 3,
};
DEFINE_ENUM_FLAGS(SubmeshDataType)

struct SubMeshHeader
{
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t dataFlags = (uint32_t)SubmeshDataType::None;
};

struct SubMeshInfo
{
    std::vector<float> positions{};
    std::vector<float> normals{};
    std::vector<float> colors{};
    std::vector<float> texCoords{};
    std::vector<uint32_t> indices{};
};

struct MeshInfo
{
    std::vector<SubMeshHeader> headers{};
    std::vector<SubMeshInfo> subMeshes{};
};

static void LoadModel(const char* path, MeshInfo* meshInfo);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: meshconv <input.obj> <output.mesh>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputPath.c_str()))
    {
        std::cerr << "Failed to load OBJ file: " << warn << err << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "Failed to open output file" << std::endl;
        return 1;
    }

    MeshInfo meshInfo{};
    LoadModel(inputPath.c_str(), &meshInfo);

    uint32_t submeshCount = meshInfo.headers.size();
    uint32_t headerDataSize = meshInfo.headers.size() * sizeof(SubMeshHeader);
    uint32_t fullHeaderSize = sizeof(uint32_t) + headerDataSize + 1;
    std::vector<char> headersData = std::vector<char>(fullHeaderSize);
    uint32_t offset = 0;
    memcpy(headersData.data() + offset, &submeshCount, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(headersData.data() + offset, meshInfo.headers.data(), headerDataSize);
    offset += headerDataSize;
    headersData[offset] = '\0';

    outFile.write(headersData.data(), fullHeaderSize);

    for (const auto& subMesh : meshInfo.subMeshes)
    {
        offset = 0;

        uint32_t submeshDataSize = 1;
        uint32_t positionSize = 0;
        uint32_t normalSize = 0;
        uint32_t colorSize = 0;
        uint32_t texcoordSize = 0;
        uint32_t indexSize = subMesh.indices.size() * sizeof(uint32_t);

        if (!subMesh.positions.empty())
        {
            positionSize = subMesh.positions.size() * sizeof(float);
            submeshDataSize++;
        }
        if (!subMesh.normals.empty())
        {
            submeshDataSize++;
            normalSize = subMesh.normals.size() * sizeof(float);
        }
        if (!subMesh.colors.empty())
        {
            submeshDataSize++;
            colorSize = subMesh.colors.size() * sizeof(float);
        }
        if (!subMesh.texCoords.empty())
        {
            submeshDataSize++;
            texcoordSize = subMesh.texCoords.size() * sizeof(float);
        }

        submeshDataSize += positionSize + normalSize + colorSize + texcoordSize + indexSize;
        std::vector<char> subMeshData = std::vector<char>(submeshDataSize);

        if (positionSize > 0)
        {
            memcpy(subMeshData.data() + offset, subMesh.positions.data(), positionSize);
            offset += positionSize;
            subMeshData[offset] = '\0';
            offset++;
        }

        if (normalSize > 0)
        {
            memcpy(subMeshData.data() + offset, subMesh.normals.data(), normalSize);
            offset += normalSize;
            subMeshData[offset] = '\0';
            offset++;
        }

        if (colorSize > 0)
        {
            memcpy(subMeshData.data() + offset, subMesh.colors.data(), colorSize);
            offset += colorSize;
            subMeshData[offset] = '\0';
            offset++;
        }

        if (texcoordSize > 0)
        {
            memcpy(subMeshData.data() + offset, subMesh.texCoords.data(), texcoordSize);
            offset += texcoordSize;
            subMeshData[offset] = '\0';
            offset++;
        }

        memcpy(subMeshData.data() + offset, subMesh.indices.data(), indexSize);
        offset += indexSize;
        subMeshData[offset] = '\0';

        outFile.write(subMeshData.data(), submeshDataSize);
    }

    outFile.close();
    std::cout << "Successfully converted " << inputPath << " to " << outputPath << std::endl;

    return 0;
}

void LoadModel(const char* path, MeshInfo* meshInfo)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path))
    {
        throw std::runtime_error(warn + err);
    }

    meshInfo->subMeshes.reserve(shapes.size());
    meshInfo->headers.reserve(shapes.size());

    for (const auto& shape : shapes)
    {
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        std::vector<Vertex> vertices{};
        std::vector<uint32_t> indices{};

        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            if (!attrib.vertices.empty())
            {
                vertex.pos =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
            }

            if (!attrib.normals.empty())
            {
                vertex.normal =
                {
                    attrib.vertices[3 * index.normal_index + 0],
                    attrib.vertices[3 * index.normal_index + 1],
                    attrib.vertices[3 * index.normal_index + 2]
                };
            }

            if (!attrib.colors.empty())
            {
                vertex.color =
                {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]
                };
            }

            if (!attrib.texcoords.empty())
            {
                vertex.texCoord =
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }

        SubMeshInfo subMeshInfo
        {
            .indices = std::move(indices)
        };

        SubmeshDataType dataFlags = SubmeshDataType::None;
        if (!attrib.vertices.empty())
        {
            subMeshInfo.positions.resize(vertices.size() * 3);
            dataFlags |= SubmeshDataType::Position;
        }
        if (!attrib.normals.empty())
        {
            subMeshInfo.normals.resize(vertices.size() * 3);
            dataFlags |= SubmeshDataType::Normal;
        }
        if (!attrib.colors.empty())
        {
            subMeshInfo.colors.resize(vertices.size() * 3);
            dataFlags |= SubmeshDataType::Color;
        }
        if (!attrib.texcoords.empty())
        {
            subMeshInfo.texCoords.resize(vertices.size() * 2);
            dataFlags |= SubmeshDataType::TexCoord;
        }

        SubMeshHeader header
        {
            .vertexCount = (uint32_t)vertices.size(),
            .indexCount = (uint32_t)subMeshInfo.indices.size(),
            .dataFlags = (uint32_t)dataFlags
        };
        meshInfo->headers.push_back(header);

        for (uint32_t i = 0; i < vertices.size(); ++i)
        {
            const auto& vertex = vertices[i];
            if (header.dataFlags & (uint32_t)SubmeshDataType::Position)
            {
                subMeshInfo.positions[i * 3 + 0] = vertex.pos.x;
                subMeshInfo.positions[i * 3 + 1] = vertex.pos.y;
                subMeshInfo.positions[i * 3 + 2] = vertex.pos.z;
            }

            if (header.dataFlags & (uint32_t)SubmeshDataType::Normal)
            {
                subMeshInfo.normals[i * 3 + 0] = vertex.normal.x;
                subMeshInfo.normals[i * 3 + 1] = vertex.normal.y;
                subMeshInfo.normals[i * 3 + 2] = vertex.normal.z;
            }

            if (header.dataFlags & (uint32_t)SubmeshDataType::Color)
            {
                subMeshInfo.colors[i * 3 + 0] = vertex.color.x;
                subMeshInfo.colors[i * 3 + 1] = vertex.color.y;
                subMeshInfo.colors[i * 3 + 2] = vertex.color.z;
            }

            if (header.dataFlags & (uint32_t)SubmeshDataType::TexCoord)
            {
                subMeshInfo.texCoords[i * 2 + 0] = vertex.texCoord.x;
                subMeshInfo.texCoords[i * 2 + 1] = vertex.texCoord.y;
            }
        }

        meshInfo->subMeshes.push_back(std::move(subMeshInfo));
    }
}