#ifndef MESHES_H
#define MESHES_H

#include <hyperflow.h>

namespace app
{
    struct AppMeshes
    {
        hf::Ref<hf::Mesh> viking_room{};
    };

    extern AppMeshes APP_MESHES;

    void MeshLoadAll();
}

#endif //MESHES_H
