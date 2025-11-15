#ifndef MATERIALS_H
#define MATERIALS_H

#include <hyperflow.h>

namespace app
{
    struct alignas(16) Mat3Padded
    {
        glm::vec4 col0;
        glm::vec4 col1;
        glm::vec4 col2;

        Mat3Padded(const glm::mat3& m)
        {
            col0 = glm::vec4(m[0], 0.0f);
            col1 = glm::vec4(m[1], 0.0f);
            col2 = glm::vec4(m[2], 0.0f);
        }
    };

    struct LitInstanceData
    {
        alignas(16) hf::mat4 modelMatrix;
        alignas(16) Mat3Padded normalMatrix;
        alignas(16) hf::vec4 color;
    };

    struct DefaultInstanceData
    {
        alignas(16) hf::mat4 modelMatrix;
        alignas(16) hf::vec4 color;
    };

    struct DefaultPushConstant
    {
        alignas(16) hf::vec4 ambientColor; //xyz specular color, w smoothness
    };

    struct AppMaterials
    {

    };

    extern AppMaterials APP_MATERIALS;

    void MaterialLoadAll();
}

#endif //MATERIALS_H
