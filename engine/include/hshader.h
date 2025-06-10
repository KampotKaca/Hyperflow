#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        explicit Shader(const ShaderCreationInfo& info);
        ~Shader();

        std::string vertLoc{}, fragLoc{};
        RenderPass renderPass{};
        ShaderSetup shaderSetup{};

        BufferAttrib* pSupportedAttribs{};
        uint32_t supportedAttribCount{};

        ShaderRasterizerOptions rasterizerOptions{};
        ShaderDepthStencilOptions depthStencilOptions{};
        ShaderBlendingOptions blendingOptions{};
        void* handle{};
    };
}

#endif //HSHADER_H
