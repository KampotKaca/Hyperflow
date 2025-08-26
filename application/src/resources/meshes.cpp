#include "resources/meshes.h"

namespace app
{
    AppMeshes APP_MESHES;

    void MeshLoadAll()
    {
        APP_MESHES.viking_room = hf::Cast<hf::Mesh>(hf::CreateAsset("viking_room.obj", hf::AssetType::Mesh));
        hf::SubmitAllBuffers();
    }
}