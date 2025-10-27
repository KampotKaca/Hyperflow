#ifndef HVKSHADER_H
#define HVKSHADER_H

#include "hvk_shared.h"
#include "hvk_renderer.h"
#include "hvk_texturepack.h"

namespace hf
{
    struct VkShader
    {
        explicit VkShader(const ir::rdr::ShaderCreationInfo_i& info);
        ~VkShader();

        ShaderLayout layout{};
        VkPipeline pipeline{};
    };

    void BindShader(const VkRenderer* rn, const VkShader* shader);
}

#endif //HVKSHADER_H
