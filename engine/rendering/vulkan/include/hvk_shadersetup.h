#ifndef HVK_SHADERSETUP_H
#define HVK_SHADERSETUP_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderSetup
    {
        explicit VkShaderSetup(const ShaderSetupDefinitionInfo& info);
        ~VkShaderSetup();

        VkPipelineLayout layout{};
        VkShaderStageFlags stageFlags{};
        uint32_t pushConstantSize{};
    };

    bool IsValidShaderSetup(ShaderSetup setup);
    URef<VkShaderSetup>& GetShaderSetup(ShaderSetup setup);
}

#endif //HVK_SHADERSETUP_H
