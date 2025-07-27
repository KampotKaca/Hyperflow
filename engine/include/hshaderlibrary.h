#ifndef HSHADERLIBRARY_H
#define HSHADERLIBRARY_H

#include "hshared.h"

namespace hf
{
    struct ShaderLibrary
    {
        explicit ShaderLibrary(const ShaderLibraryCreationInfo& info);
        ~ShaderLibrary();

        ShaderDrawOutputFormats outputFormats{};
        std::vector<ShaderLibraryVertexInputModuleInfo> vertexInputModules{};
        std::vector<ShaderLibraryPreRasterModuleInfo> preRasterModules{};
        std::vector<ShaderLibraryFragmentModuleInfo> fragmentModules{};
        std::vector<ShaderLibraryFragmentOutputModuleInfo> fragmentOutputModules{};

        void* handle{};
    };
}

#endif //HSHADERLIBRARY_H
