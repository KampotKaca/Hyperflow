#include "hshader.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "htexturepack.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    Shader::Shader(const ShaderCreationInfo& info)
        : vertLoc(info.vertexShaderLoc), fragLoc(info.fragmentShaderLoc),
          renderPass(info.renderPass), shaderSetup(info.setup),
          texturePack(info.texturePack), supportedAttribCount(info.supportedAttribCount),
          depthStencilOptions(info.depthStencilOptions), blendingOptions(info.alphaTestOptions)
    {
        uint32_t bufferSize = sizeof(BufferAttrib) * info.supportedAttribCount;
        pSupportedAttribs = (BufferAttrib*)utils::Allocate(bufferSize);
        memcpy(pSupportedAttribs, info.pSupportedAttribs, bufferSize);
        inter::rendering::CreateShader_i(this);
    }

    Shader::~Shader()
    {
        utils::Deallocate(pSupportedAttribs);
        inter::rendering::DestroyShader_i(this);
    }

    namespace shader
    {
        Ref<Shader> Create(const ShaderCreationInfo& info)
        {
            Ref<Shader> shader = MakeRef<Shader>(info);
            inter::HF.graphicsResources.shaders[(uint64_t)shader.get()] = shader;
            return shader;
        }

        void Destroy(const Ref<Shader>& shader)
        {
            if (inter::rendering::DestroyShader_i(shader.get()))
                inter::HF.graphicsResources.shaders.erase((uint64_t)shader.get());
        }

        void Destroy(const Ref<Shader>* pShaders, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto shader = pShaders[i];
                if (inter::rendering::DestroyShader_i(shader.get()))
                    inter::HF.graphicsResources.shaders.erase((uint64_t)shader.get());
            }
        }

        bool IsRunning(const Ref<Shader>& shader) { return shader->handle; }

        void Bind(const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::rendering::ShaderBindingInfo bindingInfo
            {
                .shader = shader->handle,
                .attrib = attrib,
                .bindingPoint = RenderBindingType::Graphics
            };

            inter::HF.renderingApi.api.BindShader(inter::HF.mainWindow->renderer->handle, bindingInfo);
        }

        void Bind(const Ref<Shader>& shader, BufferAttrib attrib, RenderBindingType bindingPoint)
        {
            inter::rendering::ShaderBindingInfo bindingInfo
            {
                .shader = shader->handle,
                .attrib = attrib,
                .bindingPoint = bindingPoint
            };

            inter::HF.renderingApi.api.BindShader(inter::HF.mainWindow->renderer->handle, bindingInfo);
        }

        void Bind(const Ref<Renderer>& renderer, const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::rendering::ShaderBindingInfo bindingInfo
            {
                .shader = shader->handle,
                .attrib = attrib,
                .bindingPoint = RenderBindingType::Graphics
            };

            inter::HF.renderingApi.api.BindShader(renderer->handle, bindingInfo);
        }

        void Bind(const Ref<Renderer>& renderer, const Ref<Shader>& shader, BufferAttrib attrib, RenderBindingType bindingPoint)
        {
            inter::rendering::ShaderBindingInfo bindingInfo
           {
               .shader = shader->handle,
               .attrib = attrib,
               .bindingPoint = bindingPoint
           };

            inter::HF.renderingApi.api.BindShader(renderer->handle, bindingInfo);
        }
    }

    namespace inter::rendering
    {
        bool CreateShader_i(Shader* shader)
        {
            if (shader->handle) return false;

            switch (HF.renderingApi.type)
            {
                case RenderingApiType::Vulkan:
                {
                    std::vector<char> vertexCode{}, fragmentCode{};
                    std::string vLoc = TO_RES_PATH(std::string("shaders/vulkan/") + shader->vertLoc) + ".vert.spv",
                                fLoc = TO_RES_PATH(std::string("shaders/vulkan/") + shader->fragLoc) + ".frag.spv";

                    if (!utils::FileExists(vLoc.c_str()))
                    {
                        LOG_ERROR("[Hyperflow] Unable to find vertex shader: %s", shader->vertLoc.c_str());
                        return false;
                    }

                    if (!utils::FileExists(fLoc.c_str()))
                    {
                        LOG_ERROR("[Hyperflow] Unable to find fragment shader: %s", shader->fragLoc.c_str());
                        return false;
                    }

                    utils::ReadFile(vLoc, false, vertexCode);
                    utils::ReadFile(fLoc, false, fragmentCode);

                    ShaderCreationInfo creationInfo
                    {
                        .renderPass = shader->renderPass,
                        .shaderSetup = shader->shaderSetup,
                        .supportedAttribCount = shader->supportedAttribCount,
                        .pSupportedAttribs = shader->pSupportedAttribs,
                        .vCode = vertexCode.data(),
                        .vCodeSize = (uint32_t)vertexCode.size(),
                        .fCode = fragmentCode.data(),
                        .fCodeSize = (uint32_t)fragmentCode.size(),
                        .depthStencilOptions = shader->depthStencilOptions,
                        .blendingOptions = shader->blendingOptions
                    };

                    if (shader->texturePack) creationInfo.texPack = shader->texturePack->handle;

                    shader->handle = HF.renderingApi.api.CreateShader(creationInfo);
                }
                    break;
                case RenderingApiType::Direct3D:
                    break;
                case RenderingApiType::None:
                    throw GENERIC_EXCEPT("[Hyperflow]", "Cannot create shader without loading renderer");
            }

            return true;
        }

        bool DestroyShader_i(Shader* shader)
        {
            if (shader->handle)
            {
                HF.renderingApi.api.DestroyShader(shader->handle);
                shader->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllShaders_i(bool internalOnly)
        {
            for (const auto& shader : std::ranges::views::values(HF.graphicsResources.shaders))
                DestroyShader_i(shader.get());
            if (!internalOnly) HF.graphicsResources.shaders.clear();
        }
    }

    namespace shadersetup
    {
        ShaderSetup Define(const ShaderSetupDefinitionInfo& info)
        {
            return (ShaderSetup)inter::HF.renderingApi.api.DefineShaderSetup(info);
        }

        void Bind(const Ref<Renderer>& rn, ShaderSetup setup)
        {
            inter::HF.renderingApi.api.BindShaderSetup(rn->handle, setup);
        }
    }
}