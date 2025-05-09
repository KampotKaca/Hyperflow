#ifndef HVK_SHADERSETUP_H
#define HVK_SHADERSETUP_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderSetup
    {
        VkShaderSetup(const ShaderSetupDefinitionInfo& info);
        ~VkShaderSetup();

        VkPipelineLayout layout{};
    };

    bool IsValidShaderSetup(ShaderSetup setup);
    const VkShaderSetup& GetShaderSetup(ShaderSetup setup);
}

#endif //HVK_SHADERSETUP_H
