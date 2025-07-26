#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        explicit Shader(const ShaderCreationInfo& info);
        ~Shader();

        ShaderLayout layout{};
        Ref<ShaderLibrary> library{};
        ShaderModulesInfo modules{};
        void* handle{};
    };
}

#endif //HSHADER_H
