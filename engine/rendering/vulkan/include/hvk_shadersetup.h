#ifndef HVK_SHADERSETUP_H
#define HVK_SHADERSETUP_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderSetup
    {
        explicit VkShaderSetup(const ShaderSetupDefinitionInfo& info);
        ~VkShaderSetup();
        VkShaderSetup(VkShaderSetup&& other) noexcept;

        VkPipelineLayout layout{};
    };

    bool IsValidShaderSetup(ShaderSetup setup);
    const VkShaderSetup& GetShaderSetup(ShaderSetup setup);
}

#endif //HVK_SHADERSETUP_H
