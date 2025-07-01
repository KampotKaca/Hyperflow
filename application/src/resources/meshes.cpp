#include "resources/meshes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshLoadAll()
    {
        APP_MESHES.viking_room = hf::CreateMeshAsset("viking_room.obj");
        APP_MESHES.plane = hf::CreateMeshAsset("plane.obj");
        hf::SubmitAllBuffers();
    }
}