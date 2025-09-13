#include "hmeshashared.h"
#include "hmeshconvertor.h"
#include "tiny_obj_loader.h"

namespace ml
{
    bool LoadObj(const char* path, MeshInfo* meshInfo)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        bool objRes = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path);

        if (!warn.empty()) std::cout << "[Obj Parse Warning] " << warn.c_str() << std::endl;
        if (!err.empty())  std::cout << "[Obj Parse Error] "   << err.c_str() << std::endl;

        if (!objRes)
        {
            std::cout << "Failed to parse Obj" << std::endl;
            return false;
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

            SubMeshInfo subMeshInfo{};

            auto dataFlags = hf::MeshDataType::None;
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

            if (header.dataFlags & (uint32_t)hf::MeshDataType::Position)
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

            if (header.dataFlags & (uint32_t)hf::MeshDataType::Normal)
            {
                for (uint32_t i = 0; i < vertices.size(); ++i)
                {
                    const auto& vertex = vertices[i];
                    subMeshInfo.normals[i * 3 + 0] = vertex.normal.x;
                    subMeshInfo.normals[i * 3 + 1] = vertex.normal.y;
                    subMeshInfo.normals[i * 3 + 2] = vertex.normal.z;
                }
            }

            if (header.dataFlags & (uint32_t)hf::MeshDataType::Color)
            {
                for (uint32_t i = 0; i < vertices.size(); ++i)
                {
                    const auto& vertex = vertices[i];
                    subMeshInfo.colors[i * 3 + 0] = vertex.color.x;
                    subMeshInfo.colors[i * 3 + 1] = vertex.color.y;
                    subMeshInfo.colors[i * 3 + 2] = vertex.color.z;
                }
            }

            if (header.dataFlags & (uint32_t)hf::MeshDataType::TexCoord)
            {
                for (uint32_t i = 0; i < vertices.size(); ++i)
                {
                    const auto& vertex = vertices[i];
                    subMeshInfo.texCoords[i * 2 + 0] = vertex.texCoord.x;
                    subMeshInfo.texCoords[i * 2 + 1] = vertex.texCoord.y;
                }
            }

            meshInfo->headers.push_back(header);
            meshInfo->subMeshes.push_back(std::move(subMeshInfo));
        }
        return true;
    }
}