#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        Shader(const Ref<Window>& window, const std::string& vertPath, const std::string& fragPath);
        ~Shader();

        bool isAlive{};
    };
}

#endif //HSHADER_H
