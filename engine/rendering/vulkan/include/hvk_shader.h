#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hvk_shared.h"
#include "hvk_renderer.h"

namespace hf
{
    struct VkShader
    {
        VkShader(const inter::rendering::ShaderCreationInfo& info);
        ~VkShader();

        std::unordered_map<BufferAttrib, VkPipeline> pipelines;
    };

    void BindShader(const VKRenderer* rn, VkShader* shader, BufferAttrib attrib);
}

#endif //HVKSHADER_H
