#include "resources/meshes.h"
#include "resources/bufferattributes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshLoadAll()
    {
        APP_MESHES.viking_room = hf::mesh::Create("viking_room.obj");

        hf::buffer::SubmitAll();
    }
}