#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hinternal.h"
#include "hshader.h"

namespace hf::inter::rendering
{
    struct VkShader
    {
        const Shader* shader;
    };
}

#endif //HVKSHADER_H
