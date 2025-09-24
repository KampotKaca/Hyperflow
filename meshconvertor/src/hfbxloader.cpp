#include "hmeshashared.h"
#include "hmeshinternal.h"
#include "hmeshconvertor.h"
#include "ufbx.h"

namespace ml
{
    bool LoadFbx(const char* path, MeshInfo* meshInfo)
    {
        ufbx_error error;
        ufbx_load_opts opts = {};
        ufbx_scene* scene = ufbx_load_file(path, &opts, &error);
        if (!scene)
        {
            std::cerr << "Failed to load FBX: " << error.info << std::endl;
            return false;
        }

        meshInfo->subMeshes.reserve(scene->meshes.count);
        meshInfo->headers.reserve(scene->meshes.count);

        std::vector<uint32_t> faceIndices{};

        for (size_t mi = 0; mi < scene->meshes.count; mi++)
        {
            auto* mesh = scene->meshes.data[mi];

            hf::unordered_map<Vertex, uint32_t> uniqueVertices{};
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            std::vector<uint32_t> tri_indices(mesh->max_face_triangles * 3);

            for (auto face : mesh->faces)
            {
                uint32_t num_tris = ufbx_triangulate_face(tri_indices.data(), tri_indices.size(), mesh, face);

                for (size_t i = 0; i < num_tris * 3; i++)
                {
                    uint32_t index = tri_indices[i];

                    Vertex vertex{};

                    {
                        auto position = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                        vertex.pos = hf::vec3((float_t)position.x, (float_t)position.y, (float_t)position.z);
                    }

                    if (mesh->vertex_normal.exists)
                    {
                        auto normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                        vertex.normal = hf::vec3((float_t)normal.x, (float_t)normal.y, (float_t)normal.z);
                    }

                    if (mesh->vertex_color.exists)
                    {
                        auto color = ufbx_get_vertex_vec4(&mesh->vertex_color, index);
                        vertex.color = hf::vec3((float_t)color.x, (float_t)color.y, (float_t)color.z);
                    }

                    if (mesh->vertex_color.exists)
                    {
                        auto uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                        vertex.texCoord = hf::vec2((float_t)uv.x, (float_t)uv.y);
                    }

                    if (!uniqueVertices.contains(vertex))
                    {
                        uniqueVertices[vertex] = (uint32_t)vertices.size();
                        vertices.push_back(vertex);
                    }

                    indices.push_back(uniqueVertices[vertex]);
                }
            }

            auto dataFlags = hf::MeshDataType::Position;
            if (mesh->vertex_normal.exists) dataFlags |= hf::MeshDataType::Normal;
            if (mesh->vertex_color.exists)  dataFlags |= hf::MeshDataType::Color;
            if (mesh->vertex_uv.exists)     dataFlags |= hf::MeshDataType::TexCoord;

            WriteData(meshInfo, vertices, indices, SubMeshHeader::DataType::Unique, dataFlags);
        }

        ufbx_free_scene(scene);
        return true;
    }
}