#include "meshes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshDefineAll()
    {
        hf::BufferAttribFormat formats[]
        {
            { .type = hf::BufferDataType::F32, .size = 3, },
            { .type = hf::BufferDataType::F32, .size = 3, },
            { .type = hf::BufferDataType::F32, .size = 2, },
        };

        hf::BufferAttribDefinitionInfo bufferAttribDefinitionInfo
        {
            .bindingId = 0,
            .formatCount = 3,
            .pFormats = formats
        };

        APP_MESHES.meshAttribute = hf::bufferattrib::Define(bufferAttribDefinitionInfo);

        hf::MeshCreationInfo meshInfo
        {
            .filePath = "viking_room.obj",
            .stats =
            {
                .typeFlags = hf::MeshDataType::Position | hf::MeshDataType::Color | hf::MeshDataType::TexCoord,
                .memoryType = hf::BufferMemoryType::Static,
                .bufferAttrib = APP_MESHES.meshAttribute
            }
        };
        APP_MESHES.viking_room = hf::mesh::Create(meshInfo);

        hf::buffer::SubmitAll();
    }

    void MeshLoadAll()
    {

    }
}