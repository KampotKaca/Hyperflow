#ifndef HVK_SHADERLAYOUT_H
#define HVK_SHADERLAYOUT_H

#include "hvk_shared.h"

namespace hf
{
    struct VkShaderLayout
    {
        explicit VkShaderLayout(const ShaderLayoutDefinitionInfo& info);
        ~VkShaderLayout();

        VkPipelineLayout layout{};
        PushConstantInfo pushConstantInfo{};
    };

    bool IsValidShaderLayout(ShaderLayout layout);
    URef<VkShaderLayout>& GetShaderLayout(ShaderLayout layout);
}

#endif //HVK_SHADERLAYOUT_H
