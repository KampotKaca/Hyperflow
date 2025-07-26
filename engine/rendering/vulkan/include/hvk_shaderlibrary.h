#ifndef HVK_SHADERLIBRARY_H
#define HVK_SHADERLIBRARY_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderLibrary
    {
        explicit VkShaderLibrary(const inter::rendering::ShaderLibraryCreationInfo_i& info);
        ~VkShaderLibrary();

        unordered_map<uint32_t, ShaderDrawOutputFormats> outputFormats{};
        std::vector<VkPipeline> modules{};
    };

    VkPipeline GetShaderLibraryModule(const VkShaderLibrary* lib, uint32_t moduleId);
}

#endif //HVK_SHADERLIBRARY_H
