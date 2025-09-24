#ifndef HYPERFLOW_APPLICATIONSCENEASSETS_H
#define HYPERFLOW_APPLICATIONSCENEASSETS_H

#include "hyperflow.h"

namespace app
{
    constexpr char ASSET_VIKING_ROOM_MESH[]           = "viking_room.obj";
    constexpr char ASSET_FBX_TORUS_MESH[]             = "fbx_torus.fbx";
    constexpr char ASSET_VIKING_ROOM_ALBEDO_TEXTURE[] = "viking_room.png";
    constexpr char ASSET_GREEK_HEAD_ALBEDO_TEXTURE[]  = "greek_head.jpg";

    static std::array APPLICATION_SCENE_ASSET_LOCATIONS
    {
        hf::AssetLocation
        {
            .path = ASSET_FBX_TORUS_MESH,
            .type = hf::AssetType::Mesh
        },
        hf::AssetLocation
        {
            .path = ASSET_VIKING_ROOM_MESH,
            .type = hf::AssetType::Mesh
        },
        hf::AssetLocation
        {
            .path = ASSET_VIKING_ROOM_ALBEDO_TEXTURE,
            .type = hf::AssetType::Texture
        },
        hf::AssetLocation
        {
            .path = ASSET_GREEK_HEAD_ALBEDO_TEXTURE,
            .type = hf::AssetType::Texture
        },
    };
}

#endif //HYPERFLOW_APPLICATIONSCENEASSETS_H
