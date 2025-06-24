#ifndef MATERIALS_H
#define MATERIALS_H

#include <hyperflow.h>

namespace app
{
    struct VikingRoomPushConstant
    {
        hf::mat4 modelMatrix;
        uint32_t materialIndex;
    };

    struct VikingRoomMaterialInfo
    {
        hf::vec4 color{};
    };

    struct AppMaterials
    {
        VikingRoomMaterialInfo viking_room_info{};
        hf::Ref<hf::Material> viking_room{};
    };

    extern AppMaterials APP_MATERIALS;

    void MaterialLoadAll();
}

#endif //MATERIALS_H
