#include "hshader.h"

#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    Shader::Shader(const ShaderCreationInfo& info)
    {
        vPath = info.vertexShaderLoc;
        fPath = info.fragmentShaderLoc;
        std::vector<char> vertexCode{}, fragmentCode{};
        utils::ReadFile(TO_RES_PATH(info.vertexShaderLoc), vertexCode);
        utils::ReadFile(TO_RES_PATH(info.fragmentShaderLoc), fragmentCode);

        inter::rendering::ShaderCreationInfo creationInfo
        {
            .supportedAttribCount = info.supportedAttribCount,
            .pSupportedAttribs = info.pSupportedAttribs,
            .vCode = vertexCode.data(),
            .vCodeSize = (uint32_t)vertexCode.size(),
            .fCode = fragmentCode.data(),
            .fCodeSize = (uint32_t)fragmentCode.size(),
        };

        handle = inter::rendering::CreateShader(creationInfo);
    }

    Shader::~Shader()
    {
        inter::rendering::DestroyShader_i(this);
    }

    namespace shader
    {
        Ref<Shader> Create(const ShaderCreationInfo& info)
        {
            Ref<Shader> shader = MakeRef<Shader>(info);
            inter::HF.shaders[shader.get()] = shader;
            return shader;
        }

        void Destroy(const Ref<Shader>& shader)
        {
            inter::rendering::WaitForRendering();
            if (inter::rendering::DestroyShader_i(shader.get()))
                inter::HF.shaders.erase(shader.get());
        }

        void Destroy(const Ref<Shader>* pShaders, uint32_t count)
        {
            inter::rendering::WaitForRendering();
            for (uint32_t i = 0; i < count; i++)
            {
                auto shader = pShaders[i];
                if (inter::rendering::DestroyShader_i(shader.get()))
                    inter::HF.shaders.erase(shader.get());
            }
        }

        void DestroyAll()
        {
            inter::rendering::WaitForRendering();
            for (const auto& shader : std::ranges::views::values(inter::HF.shaders))
                inter::rendering::DestroyShader_i(shader.get());
            inter::HF.shaders.clear();
        }

        bool IsRunning(const Ref<Shader>& shader) { return shader->handle; }

        void Bind(const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::rendering::BindShader(inter::HF.mainWindow->renderer.get(), shader.get(), attrib);
        }

        void Bind(const Ref<Renderer>& renderer, const Ref<Shader>& shader, BufferAttrib attrib)
        {
            inter::rendering::BindShader(renderer->handle, shader->handle, attrib);
        }
    }

    namespace inter::rendering
    {
        bool DestroyShader_i(Shader* shader)
        {
            if (shader->handle)
            {
                DestroyShader(shader->handle);
                shader->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}