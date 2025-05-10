#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hvk_shared.h"
#include "hvk_renderer.h"
#include "hvk_texturepack.h"

namespace hf
{
    struct VkShader
    {
        VkShader(const inter::rendering::ShaderCreationInfo& info);
        ~VkShader();

        ShaderSetup shaderSetup{};
        VkTexturePack* texturePack{};
        std::unordered_map<BufferAttrib, VkPipeline> pipelines;
    };

    void BindShader(VkRenderer* rn, VkShader* shader, BufferAttrib attrib);
}

#endif //HVKSHADER_H
