#include "hmodelinternal.h"
#include "tiny_obj_loader.h"

namespace ml
{
    bool LoadObj(const char* path, ModelInfo* meshInfo)
    {
        tinyobj::attrib_t attrib;
        hf::List<tinyobj::shape_t> shapes;
        hf::List<tinyobj::material_t> materials;
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
            hf::Dictionary<Vertex, uint32_t> uniqueVertices{};
            hf::List<Vertex> vertices{};
            hf::List<uint32_t> indices{};

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

            auto dataFlags = hf::MeshDataType::None;
            if (!attrib.vertices.empty())  dataFlags |= hf::MeshDataType::Position;
            if (!attrib.normals.empty())   dataFlags |= hf::MeshDataType::Normal;
            if (!attrib.colors.empty())    dataFlags |= hf::MeshDataType::Color;
            if (!attrib.texcoords.empty()) dataFlags |= hf::MeshDataType::TexCoord;

            WriteData(meshInfo, vertices, indices, dataFlags, 0, 0);
        }
        return true;
    }
}