#ifndef MATERIALS_H
#define MATERIALS_H

#include <hyperflow.h>

namespace app
{
    struct AppMaterials
    {
        hf::Ref<hf::Material> viking_room{};
        hf::Ref<hf::Material> axis_lines{};
    };

    extern AppMaterials APP_MATERIALS;

    void MaterialLoadAll();
}

#endif //MATERIALS_H
