#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        Shader(const std::string& vertPath, const std::string& fragPath);
        ~Shader();

        std::string vPath{}, fPath{};
        bool isRunning{};
        void* handle{};
    };
}

#endif //HSHADER_H
