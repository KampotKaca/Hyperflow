#ifndef MATERIALS_H
#define MATERIALS_H

#include <hyperflow.h>

namespace app
{
    struct DefaultInstanceData
    {
        alignas(16) hf::mat4 modelMatrix;
        alignas(16) hf::vec4 color;
    };

    struct DefaultPushConstant
    {
        alignas(16) hf::vec4 phongData; //xyz specular color, w smoothness
    };

    struct AppMaterials
    {

    };

    extern AppMaterials APP_MATERIALS;

    void MaterialLoadAll();
}

#endif //MATERIALS_H
