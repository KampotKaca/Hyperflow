#ifndef HSHADER_H
#define HSHADER_H

#include "hshared.h"

namespace hf
{
    struct Shader
    {
        explicit Shader(const ShaderCreationInfo& info);
        ~Shader();

        static Ref<Shader> Create(const ShaderCreationInfo& info);
        static void Destroy(const Ref<Shader>* pShaders, uint32_t count);

        void Destroy();
        [[nodiscard]] bool IsRunning() const;

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
