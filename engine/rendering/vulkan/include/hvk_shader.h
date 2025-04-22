#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hvk_shared.h"
#include "hvk_renderer.h"

namespace hf
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
        VkShader(const inter::rendering::ShaderCreationInfo& info);
        ~VkShader();

        std::unordered_map<BufferAttrib, VkPipeline> pipelines;
    };

    void BindShader(const VKRenderer* rn, VkShader* shader, BufferAttrib attrib);
}

#endif //HVKSHADER_H
