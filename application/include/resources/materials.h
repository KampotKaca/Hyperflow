#ifndef MATERIALS_H
#define MATERIALS_H

#include <hyperflow.h>

namespace app
{
    struct DefaultPushConstant
    {
        hf::mat4 modelMatrix;
        hf::vec4 color;
    };

    struct AppMaterials
    {

    };

    extern AppMaterials APP_MATERIALS;

    void MaterialLoadAll();
}

#endif //MATERIALS_H
