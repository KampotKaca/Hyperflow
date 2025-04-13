#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hinternal.h"
#include "hshader.h"
#include <vulkan/vulkan.h>

namespace hf::inter::rendering
{
    struct VkShader
    {
        VkShader(const ShaderCreationInfo& info, const Shader* shader);
        ~VkShader();

        const Shader* shader{};
        VkPipelineLayout pipelineLayout{};
        VkPipeline pipeline{};
    };
}

#endif //HVKSHADER_H
