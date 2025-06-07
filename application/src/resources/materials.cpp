#include "resources/materials.h"
#include "resources/shaders.h"

namespace app
{
    AppMaterials APP_MATERIALS;

    void MaterialLoadAll()
    {
        {
            hf::MaterialCreationInfo materialInfo
            {
                .shader = APP_SHADERS.viking_room,
                .sizeInBytes = 0
            };
            APP_MATERIALS.viking_room = hf::material::Create(materialInfo);
        }

        {
            hf::MaterialCreationInfo materialInfo
            {
                .shader = APP_SHADERS.axis_lines,
                .sizeInBytes = 0
            };
            APP_MATERIALS.axis_lines = hf::material::Create(materialInfo);
        }
    }
}