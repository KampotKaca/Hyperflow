#include "hshader.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    Shader::Shader(const ShaderCreationInfo& info)
    {
        vertLoc = std::string(info.vertexShaderLoc);
        fragLoc = std::string(info.fragmentShaderLoc);

        memcpy(&creationInfo, &info, sizeof(ShaderCreationInfo));
        uint32_t bufferSize = sizeof(BufferAttrib) * info.supportedAttribCount;
        creationInfo.pSupportedAttribs = (BufferAttrib*)utils::Allocate(bufferSize);
        memcpy(creationInfo.pSupportedAttribs, info.pSupportedAttribs, bufferSize);
        inter::rendering::CreateShader_i(this);
    }

    Shader::~Shader()
    {
        utils::Deallocate(creationInfo.pSupportedAttribs);
        inter::rendering::DestroyShader_i(this);
    }

    namespace shader
    {
        Ref<Shader> Create(const ShaderCreationInfo& info)
        {
            Ref<Shader> shader = MakeRef<Shader>(info);
            inter::HF.graphicsResources.shaders[shader.get()] = shader;
            return shader;
        }

        void Destroy(const Ref<Shader>& shader)
        {
            if (inter::rendering::DestroyShader_i(shader.get()))
                inter::HF.graphicsResources.shaders.erase(shader.get());
        }

        void Destroy(const Ref<Shader>* pShaders, uint32_t count)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                auto shader = pShaders[i];
                if (inter::rendering::DestroyShader_i(shader.get()))
                    inter::HF.graphicsResources.shaders.erase(shader.get());
            }
        }

        void DestroyAll(bool internalOnly)
        {
            for (const auto& shader : std::ranges::views::values(inter::HF.graphicsResources.shaders))
                inter::rendering::DestroyShader_i(shader.get());
            if (!internalOnly) inter::HF.graphicsResources.shaders.clear();
        }

        bool IsRunning(const Ref<Shader>& shader) { return shader->handle; }

        void Bind(const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::HF.renderingApi.api.BindShader(inter::HF.mainWindow->renderer.get(), shader.get(), attrib);
        }

        void Bind(const Ref<Renderer>& renderer, const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::HF.renderingApi.api.BindShader(renderer->handle, shader->handle, attrib);
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
                    utils::ReadFile(TO_RES_PATH(std::string("shaders/vulkan/") + shader->vertLoc) + ".vert.spv", vertexCode);
                    utils::ReadFile(TO_RES_PATH(std::string("shaders/vulkan/") + shader->fragLoc) + ".frag.spv", fragmentCode);

                    ShaderCreationInfo creationInfo
                    {
                        .uniformStorage = shader->creationInfo.uniformStorage,
                        .supportedAttribCount = shader->creationInfo.supportedAttribCount,
                        .pSupportedAttribs = shader->creationInfo.pSupportedAttribs,
                        .vCode = vertexCode.data(),
                        .vCodeSize = (uint32_t)vertexCode.size(),
                        .fCode = fragmentCode.data(),
                        .fCodeSize = (uint32_t)fragmentCode.size(),
                    };

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
    }
}