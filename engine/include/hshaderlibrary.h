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
        Dictionary<std::string, uint32_t> vertexInputModules{};
        Dictionary<std::string, uint32_t> preRasterModules{};
        Dictionary<std::string, uint32_t> fragmentModules{};
        Dictionary<std::string, uint32_t> fragmentOutputModules{};
        void* handle{};
    };
}

#endif //HSHADERLIBRARY_H
