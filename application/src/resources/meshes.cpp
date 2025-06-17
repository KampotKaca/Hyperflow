#include "resources/meshes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshLoadAll()
    {
        APP_MESHES.viking_room = hf::Mesh::Create("viking_room.obj");
        hf::Buffer::SubmitAll();
    }
}