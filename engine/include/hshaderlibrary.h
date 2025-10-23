#ifndef HSHADERLIBRARY_H
#define HSHADERLIBRARY_H

#include "hshared.h"

namespace hf
{
    struct ShaderLibrary
    {
        explicit ShaderLibrary(const ShaderLibraryCreationInfo& info);
        ~ShaderLibrary();

        std::string name{};
        unordered_map<std::string, uint32_t> vertexInputModules{};
        unordered_map<std::string, uint32_t> preRasterModules{};
        unordered_map<std::string, uint32_t> fragmentModules{};
        unordered_map<std::string, uint32_t> fragmentOutputModules{};
        void* handle{};
    };
}

#endif //HSHADERLIBRARY_H
