#define GLM_ENABLE_EXPERIMENTAL
#include "tiny_obj_loader.h"
#include <glm/gtx/hash.hpp>
#include "hmeshconvertor.h"
#include <lz4.h>

#include "hyperflow.h"

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

static void LoadModel(const char* path, MeshInfo* meshInfo);
static bool WriteMesh(const MeshInfo& meshInfo, const std::string& outputFilePath);

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(true);

    if (argc != 3)
    {
        std::cout << "Usage: meshconv <input.obj> <output.mesh>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    try
    {
        MeshInfo meshInfo{};
        LoadModel(inputPath.c_str(), &meshInfo);

        if (WriteMesh(meshInfo, outputPath))
        {
            std::cout << "Successfully converted " << inputPath << " to " << outputPath << std::endl;
            return 0;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

bool WriteMesh(const MeshInfo& meshInfo, const std::string& outputFilePath)
{
    uint32_t submeshCount = meshInfo.headers.size();
    uint32_t headerDataSize = meshInfo.headers.size() * sizeof(SubMeshHeader);
    uint32_t fullHeaderSize = sizeof(uint32_t) + headerDataSize + 1;
    uint32_t offset = 0;
    std::vector<char> headerData = std::vector<char>(fullHeaderSize);

    memcpy(headerData.data() + offset, &submeshCount, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(headerData.data() + offset, meshInfo.headers.data(), headerDataSize);
    offset += headerDataSize;
    headerData[offset] = '\0';

    offset = 0;
    std::vector<char> meshData{};
    for (uint32_t i = 0; i < submeshCount; i++)
    {
        auto& subMesh = meshInfo.subMeshes[i];
        offset = 0;

        uint32_t submeshDataSize = 1;
        uint32_t positionSize = 0;
        uint32_t normalSize = 0;
        uint32_t colorSize = 0;
        uint32_t texcoordSize = 0;
        uint32_t indexSize = subMesh.indices.size();

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
        uint64_t oldSize = meshData.size();
        meshData.resize(meshData.size() + submeshDataSize);

        if (positionSize > 0)
        {
            memcpy(meshData.data() + oldSize + offset, subMesh.positions.data(), positionSize);
            offset += positionSize;
            meshData[oldSize + offset] = '\0';
            offset++;
        }

        if (normalSize > 0)
        {
            memcpy(meshData.data() + oldSize + offset, subMesh.normals.data(), normalSize);
            offset += normalSize;
            meshData[offset + oldSize] = '\0';
            offset++;
        }

        if (colorSize > 0)
        {
            memcpy(meshData.data() + oldSize + offset, subMesh.colors.data(), colorSize);
            offset += colorSize;
            meshData[offset + oldSize] = '\0';
            offset++;
        }

        if (texcoordSize > 0)
        {
            memcpy(meshData.data() + oldSize + offset, subMesh.texCoords.data(), texcoordSize);
            offset += texcoordSize;
            meshData[offset + oldSize] = '\0';
            offset++;
        }

        memcpy(meshData.data() + oldSize + offset, subMesh.indices.data(), indexSize);
        offset += indexSize;
        meshData[offset + oldSize] = '\0';
    }

    const int maxCompressedSize = LZ4_compressBound((int)meshData.size());
    auto compressedBuffer = (char*)malloc(maxCompressedSize);
    if (!compressedBuffer) std::cerr << "Failed to allocate memory" << std::endl;

    auto dataSize = LZ4_compress_default(meshData.data(), compressedBuffer, (int)meshData.size(), maxCompressedSize);

    if (dataSize <= 0)
    {
        std::cerr << "Failed to compress file" << std::endl;
        free(compressedBuffer);
        return false;
    }

    std::ofstream outFile(outputFilePath, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "Failed to open output file" << std::endl;
        free(compressedBuffer);
        return false;
    }

    outFile.write(headerData.data(), fullHeaderSize);
    outFile.write(compressedBuffer, dataSize);
    outFile.close();

    free(compressedBuffer);
    return true;
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
        hf::unordered_map<Vertex, uint32_t> uniqueVertices{};
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
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
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

            if (!uniqueVertices.contains(vertex))
            {
                uniqueVertices[vertex] = (uint32_t)vertices.size();
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }

        SubMeshInfo subMeshInfo
        {
        };

        hf::MeshDataType dataFlags = hf::MeshDataType::None;
        if (!attrib.vertices.empty())
        {
            subMeshInfo.positions.resize(vertices.size() * 3);
            dataFlags |= hf::MeshDataType::Position;
        }
        if (!attrib.normals.empty())
        {
            subMeshInfo.normals.resize(vertices.size() * 3);
            dataFlags |= hf::MeshDataType::Normal;
        }
        if (!attrib.colors.empty())
        {
            subMeshInfo.colors.resize(vertices.size() * 3);
            dataFlags |= hf::MeshDataType::Color;
        }
        if (!attrib.texcoords.empty())
        {
            subMeshInfo.texCoords.resize(vertices.size() * 2);
            dataFlags |= hf::MeshDataType::TexCoord;
        }

        SubMeshHeader header
        {
            .vertexCount = (uint32_t)vertices.size(),
            .indexCount = (uint32_t)indices.size(),
            .dataFlags = (uint32_t)dataFlags,
        };

        if (vertices.size() <= 65535)
        {
            header.indexFormat = (uint32_t)hf::MeshIndexFormat::U16;
            subMeshInfo.indices = std::vector<char>(indices.size() * 2);
            for (uint32_t i = 0; i < indices.size(); ++i)
            {
                auto result = (uint16_t)indices[i];
                memcpy(&subMeshInfo.indices[i * 2], &result, sizeof(uint16_t));
            }
        }
        else
        {
            header.indexFormat = (uint32_t)hf::MeshIndexFormat::U32;
            subMeshInfo.indices = std::vector<char>(indices.size() * 4);
            memcpy(subMeshInfo.indices.data(), indices.data(), sizeof(uint32_t) * indices.size());
        }

        meshInfo->headers.push_back(header);

        for (uint32_t i = 0; i < vertices.size(); ++i)
        {
            const auto& vertex = vertices[i];
            if (header.dataFlags & (uint32_t)hf::MeshDataType::Position)
            {
                subMeshInfo.positions[i * 3 + 0] = vertex.pos.x;
                subMeshInfo.positions[i * 3 + 1] = vertex.pos.y;
                subMeshInfo.positions[i * 3 + 2] = vertex.pos.z;
            }

            if (header.dataFlags & (uint32_t)hf::MeshDataType::Normal)
            {
                subMeshInfo.normals[i * 3 + 0] = vertex.normal.x;
                subMeshInfo.normals[i * 3 + 1] = vertex.normal.y;
                subMeshInfo.normals[i * 3 + 2] = vertex.normal.z;
            }

            if (header.dataFlags & (uint32_t)hf::MeshDataType::Color)
            {
                subMeshInfo.colors[i * 3 + 0] = vertex.color.x;
                subMeshInfo.colors[i * 3 + 1] = vertex.color.y;
                subMeshInfo.colors[i * 3 + 2] = vertex.color.z;
            }

            if (header.dataFlags & (uint32_t)hf::MeshDataType::TexCoord)
            {
                subMeshInfo.texCoords[i * 2 + 0] = vertex.texCoord.x;
                subMeshInfo.texCoords[i * 2 + 1] = vertex.texCoord.y;
            }
        }

        meshInfo->subMeshes.push_back(std::move(subMeshInfo));
    }
}