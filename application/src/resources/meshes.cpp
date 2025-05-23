#include "resources/meshes.h"
#include "resources/bufferattributes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshLoadAll()
    {
        hf::MeshCreationInfo meshInfo
        {
            .filePath = "viking_room.obj",
            .stats =
            {
                .typeFlags = hf::MeshDataType::Position | hf::MeshDataType::Color | hf::MeshDataType::TexCoord,
                .memoryType = hf::BufferMemoryType::Static,
                .bufferAttrib = APP_BUFFER_ATTRIBUTES.pctAttribute,
            }
        };
        APP_MESHES.viking_room = hf::mesh::Create(meshInfo);

        hf::buffer::SubmitAll();
    }
}