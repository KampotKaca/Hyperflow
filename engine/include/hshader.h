#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        Shader(const ShaderCreationInfo& info);
        ~Shader();

        std::string vertLoc{}, fragLoc{};
        UniformStorage uniformStorage{};

        BufferAttrib* pSupportedAttribs{};
        uint32_t supportedAttribCount{};
        void* handle{};
    };
}

#endif //HSHADER_H
