#include "hmodelinternal.h"
#include "ufbx.h"

namespace ml
{
    bool LoadFbx(const char* path, ModelInfo* meshInfo)
    {
        //Load the fbx via ufbx api
        ufbx_error error;
        ufbx_load_opts opts = {};
        ufbx_scene* scene = ufbx_load_file(path, &opts, &error);
        if (!scene)
        {
            std::cerr << "Failed to load FBX: " << error.info << std::endl;
            return false;
        }

        //Reserve space for all the necessary data
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

            auto skinDeformationCount = mesh->skin_deformers.count;
            if (skinDeformationCount > MAX_DEFORMERS_PER_VERTEX) skinDeformationCount = MAX_DEFORMERS_PER_VERTEX;
            auto blendDeformationCount = mesh->skin_deformers.count;
            if (blendDeformationCount > MAX_DEFORMERS_PER_VERTEX) blendDeformationCount = MAX_DEFORMERS_PER_VERTEX;

            for (auto face : mesh->faces)
            {
                uint32_t num_tris = ufbx_triangulate_face(tri_indices.data(), tri_indices.size(), mesh, face);

                for (size_t i = 0; i < num_tris * 3; i++)
                {
                    uint32_t index = tri_indices[i];
                    uint32_t vIndex = mesh->vertex_indices[index];

                    Vertex vertex{};

                    { //Position
                        auto position = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                        vertex.pos = hf::vec3((float_t)position.x, (float_t)position.y, (float_t)position.z);
                    }

                    if (mesh->vertex_normal.exists) //Normal
                    {
                        auto normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                        vertex.normal = hf::vec3((float_t)normal.x, (float_t)normal.y, (float_t)normal.z);
                    }

                    if (mesh->vertex_color.exists) //Color
                    {
                        auto color = ufbx_get_vertex_vec4(&mesh->vertex_color, index);
                        vertex.color = hf::vec3((float_t)color.x, (float_t)color.y, (float_t)color.z);
                    }

                    if (mesh->vertex_uv.exists) //Texcoord0
                    {
                        auto uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                        vertex.texCoord = hf::vec2((float_t)uv.x, (float_t)uv.y);
                    }

                    for (uint32_t sk = 0; sk < skinDeformationCount; sk++) //Skinning
                    {
                        auto* skin = mesh->skin_deformers.data[sk];
                        auto skinVertex = skin->vertices.data[vIndex];
                        auto numWeights = skinVertex.num_weights;

                        auto& skinDeformer = vertex.skinDeformers[sk];

                        if (numWeights > MAX_DEFORMATIONS_PER_VERTEX) numWeights = MAX_DEFORMATIONS_PER_VERTEX;

                        float totalWeight = 0.0f;
                        for (size_t w = 0; w < numWeights; w++) //Setting all the Bones and weights for each vertex
                        {
                            auto skin_weight = skin->weights.data[skinVertex.weight_begin + w];
                            skinDeformer.bones[w] = skin_weight.cluster_index;
                            skinDeformer.weights[w] = (float)skin_weight.weight;
                            totalWeight += (float)skin_weight.weight;
                        }

                        //normalizing weights since fbx doesn't
                        for (size_t w = 0; w < numWeights; w++) skinDeformer.weights[w] /= totalWeight;
                    }

                    for (uint32_t bl = 0; bl < blendDeformationCount; bl++) //Blending
                    {
                        auto* blend = mesh->blend_deformers.data[bl];
                        auto numBlends = blend->channels.count;
                        if (numBlends > MAX_DEFORMATIONS_PER_VERTEX) numBlends = MAX_DEFORMATIONS_PER_VERTEX;

                        auto& blendDeformer = vertex.blendDeformers[bl];
                        blendDeformer.deformationCount = 0;

                        for (size_t b = 0; b < numBlends; b++)
                        {
                            auto* channel = blend->channels.data[b];
                            auto* shape = channel->target_shape;

                            if (!shape) throw std::runtime_error("Missing shape while loading fbx!");

                            auto blendOffset = ufbx_get_blend_shape_vertex_offset(shape, vIndex);
                            blendDeformer.offsets[b] = glm::vec3(blendOffset.x, blendOffset.y, blendOffset.z);
                        }
                    }

                    //add the new vertex, but check first if the same vertex already exists and skip if so.
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

            WriteData(meshInfo, vertices, indices, dataFlags, skinDeformationCount, blendDeformationCount);
        }

        ufbx_free_scene(scene);
        return true;
    }
}