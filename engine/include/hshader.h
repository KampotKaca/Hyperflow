#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        Shader(const ShaderCreationInfo& info);
        ~Shader();

        std::string vPath{}, fPath{};
        void* handle{};
    };
}

#endif //HSHADER_H
