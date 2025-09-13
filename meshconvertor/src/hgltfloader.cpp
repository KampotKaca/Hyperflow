#include "hmeshashared.h"
#include "hmeshconvertor.h"
#include "tiny_gltf.h"

namespace ml
{
    bool LoadGltf(const char* path, MeshInfo* meshInfo, bool isGltf)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool gltfResult = false;

        if (isGltf) gltfResult = loader.LoadASCIIFromFile(&model, &err, &warn, path);
        else gltfResult = loader.LoadBinaryFromFile(&model, &err, &warn, path);

        if (!warn.empty()) std::cout << "[Gltf Parse Warning] " << warn.c_str() << std::endl;
        if (!err.empty())  std::cout << "[Gltf Parse Error] "   << err.c_str() << std::endl;

        if (!gltfResult)
        {
            std::cout << "Failed to parse Gltf" << std::endl;
            return false;
        }

        meshInfo->subMeshes.reserve(model.meshes.size());
        meshInfo->headers.reserve(model.meshes.size());

        for (const auto& mesh : model.meshes)
        {
            for (const auto& prim : mesh.primitives)
            {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                // --- extract indices ---
                if (prim.indices >= 0)
                {
                    const tinygltf::Accessor& accessor = model.accessors[prim.indices];
                    const tinygltf::BufferView& bv     = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buf        = model.buffers[bv.buffer];
                    const uint8_t* dataPtr             = buf.data.data() + bv.byteOffset + accessor.byteOffset;

                    for (size_t i = 0; i < accessor.count; i++)
                    {
                        uint32_t index = 0;
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                                index = *((uint8_t*)(dataPtr + i * accessor.ByteStride(bv)));
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                                index = *((uint16_t*)(dataPtr + i * accessor.ByteStride(bv)));
                                break;
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                                index = *((uint32_t*)(dataPtr + i * accessor.ByteStride(bv)));
                                break;
                            default: break;
                        }
                        indices.push_back(index);
                    }
                }

                // --- extract vertex attributes ---
                size_t vertexCount = 0;
                if (prim.attributes.contains("POSITION")) vertexCount = model.accessors[prim.attributes.at("POSITION")].count;

                std::vector positions(vertexCount, hf::vec3{ 0, 0, 0 });
                std::vector normals  (vertexCount, hf::vec3{ 0, 0, 0 });
                std::vector colors   (vertexCount, hf::vec3{ 1, 1, 1 });
                std::vector texcoords(vertexCount, hf::vec2{ 0, 0 });

                auto ReadAccessorVec3 = [&](std::vector<hf::vec3>& dst, const tinygltf::Accessor& accessor)
                {
                    const tinygltf::BufferView& bv = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buf = model.buffers[bv.buffer];
                    const uint8_t* dataPtr = buf.data.data() + bv.byteOffset + accessor.byteOffset;
                    for (size_t i = 0; i < accessor.count; i++)
                    {
                        auto* v = (const float*)(dataPtr + i * accessor.ByteStride(bv));
                        dst[i] = glm::vec3(v[0], v[1], v[2]);
                    }
                };

                auto ReadAccessorVec2 = [&](std::vector<hf::vec2>& dst, const tinygltf::Accessor& accessor)
                {
                    const tinygltf::BufferView& bv = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buf = model.buffers[bv.buffer];
                    const uint8_t* dataPtr = buf.data.data() + bv.byteOffset + accessor.byteOffset;
                    for (size_t i = 0; i < accessor.count; i++)
                    {
                        auto* v = (const float*)(dataPtr + i * accessor.ByteStride(bv));
                        dst[i] = hf::vec2(v[0], 1.0f - v[1]); // flip V like your obj loader
                    }
                };

                if (prim.attributes.contains("POSITION"))   ReadAccessorVec3(positions, model.accessors[prim.attributes.at("POSITION")]);
                if (prim.attributes.contains("NORMAL"))     ReadAccessorVec3(normals,   model.accessors[prim.attributes.at("NORMAL")]);
                if (prim.attributes.contains("COLOR_0"))    ReadAccessorVec3(colors,    model.accessors[prim.attributes.at("COLOR_0")]);
                if (prim.attributes.contains("TEXCOORD_0")) ReadAccessorVec2(texcoords, model.accessors[prim.attributes.at("TEXCOORD_0")]);

                // --- fill SubMeshInfo ---
                SubMeshInfo subMeshInfo{};
                SubMeshHeader header{};

                header.vertexCount = (uint32_t)vertexCount;
                header.indexCount  = (uint32_t)indices.size();

                if (!positions.empty())
                {
                    subMeshInfo.positions.resize(vertexCount * 3);
                    header.dataFlags |= (uint32_t)hf::MeshDataType::Position;
                }
                if (!normals.empty())
                {
                    subMeshInfo.normals.resize(vertexCount * 3);
                    header.dataFlags |= (uint32_t)hf::MeshDataType::Normal;
                }
                if (!colors.empty())
                {
                    subMeshInfo.colors.resize(vertexCount * 3);
                    header.dataFlags |= (uint32_t)hf::MeshDataType::Color;
                }
                if (!texcoords.empty())
                {
                    subMeshInfo.texCoords.resize(vertexCount * 2);
                    header.dataFlags |= (uint32_t)hf::MeshDataType::TexCoord;
                }

            // choose index format
            if (indices.size() > 0)
            {
                if (vertexCount <= 65535)
                {
                    header.indexFormat = (uint32_t)hf::MeshIndexFormat::U16;
                    subMeshInfo.indices.resize(indices.size() * 2);
                    for (size_t i = 0; i < indices.size(); i++)
                    {
                        uint16_t idx16 = (uint16_t)indices[i];
                        memcpy(&subMeshInfo.indices[i * 2], &idx16, 2);
                    }
                }
                else
                {
                    header.indexFormat = (uint32_t)hf::MeshIndexFormat::U32;
                    subMeshInfo.indices.resize(indices.size() * 4);
                    memcpy(subMeshInfo.indices.data(), indices.data(), indices.size() * 4);
                }
            }

            // fill vertex arrays
            for (size_t i = 0; i < vertexCount; i++)
            {
                if (header.dataFlags & (uint32_t)hf::MeshDataType::Position)
                {
                    subMeshInfo.positions[i * 3 + 0] = positions[i].x;
                    subMeshInfo.positions[i * 3 + 1] = positions[i].y;
                    subMeshInfo.positions[i * 3 + 2] = positions[i].z;
                }
                if (header.dataFlags & (uint32_t)hf::MeshDataType::Normal)
                {
                    subMeshInfo.normals[i * 3 + 0] = normals[i].x;
                    subMeshInfo.normals[i * 3 + 1] = normals[i].y;
                    subMeshInfo.normals[i * 3 + 2] = normals[i].z;
                }
                if (header.dataFlags & (uint32_t)hf::MeshDataType::Color)
                {
                    subMeshInfo.colors[i * 3 + 0] = colors[i].r;
                    subMeshInfo.colors[i * 3 + 1] = colors[i].g;
                    subMeshInfo.colors[i * 3 + 2] = colors[i].b;
                }
                if (header.dataFlags & (uint32_t)hf::MeshDataType::TexCoord)
                {
                    subMeshInfo.texCoords[i * 2 + 0] = texcoords[i].x;
                    subMeshInfo.texCoords[i * 2 + 1] = texcoords[i].y;
                }
            }

            // compute bounding box
            if (vertexCount > 0)
            {
                header.volume.min = positions[0];
                header.volume.max = positions[0];
                for (size_t i = 1; i < positions.size(); i++) header.volume.Encapsulate(positions[i]);
            }

            meshInfo->headers.push_back(header);
            meshInfo->subMeshes.push_back(std::move(subMeshInfo));
        }
    }

        return true;
    }
}