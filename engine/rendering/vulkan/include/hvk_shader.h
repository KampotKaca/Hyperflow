#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hinternal.h"
#include <vulkan/vulkan.h>

namespace hf::inter::rendering
{
    enum class PipelineBlendType { None, Alpha, Logical };
    struct VkPipelineInfo
    {
        uint32_t stageCount = 0;
        VkPipelineShaderStageCreateInfo* pStages = nullptr;
        PipelineBlendType blendingMode = PipelineBlendType::None;
        VkLogicOp blendingOp = VK_LOGIC_OP_XOR; //Setting will be used only if you use Logical Blending
        BufferAttrib attrib{};
        VkPipelineLayout layout{};
        VkRenderPass renderPass{};
    };

    struct VkShader
    {
        VkShader(const ShaderCreationInfo& info);
        ~VkShader();

        std::unordered_map<BufferAttrib, VkPipeline> pipelines;
    };
}

#endif //HVKSHADER_H
