#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hvk_shared.h"
#include "hvk_renderer.h"
#include "hvk_texturepack.h"

namespace hf
{
    struct VkShader
    {
        explicit VkShader(const inter::rendering::ShaderCreationInfo& info);
        ~VkShader();

        ShaderLayout layout{};
        unordered_map<BufferAttrib, VkPipeline> pipelines{};
    };

    void BindShader(VkRenderer* rn, VkShader* shader, BufferAttrib attrib, RenderBindingType bindingType);
}

#endif //HVKSHADER_H
