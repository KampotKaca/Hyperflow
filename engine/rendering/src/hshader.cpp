#include "hshader.h"

#include <ranges>

#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    Shader::Shader(const std::string& vertPath, const std::string& fragPath)
    {
        vPath = vertPath;
        fPath = fragPath;
        std::vector<char> vertexCode{}, fragmentCode{};
        utils::ReadFile(TO_RES_PATH(vertPath), vertexCode);
        utils::ReadFile(TO_RES_PATH(fragPath), fragmentCode);

        inter::rendering::ShaderCreationInfo creationInfo
        {
            .vCode = vertexCode.data(),
            .vCodeSize = (uint32_t)vertexCode.size(),
            .fCode = fragmentCode.data(),
            .fCodeSize = (uint32_t)fragmentCode.size(),
        };

        handle = inter::rendering::CreateShader(creationInfo, this);
        isRunning = true;
    }

    Shader::~Shader()
    {
        inter::shader::Destroy(this);
    }

    namespace shader
    {
        Ref<Shader> Create(const std::string& vertPath, const std::string& fragPath)
        {
            Ref<Shader> shader = MakeRef<Shader>(vertPath, fragPath);
            inter::HF.shaders[shader.get()] = shader;
            return shader;
        }

        void Destroy(const Ref<Shader>& shader)
        {
            if (inter::shader::Destroy(shader.get()))
                inter::HF.shaders.erase(shader.get());
        }

        void DestroyAll()
        {
            for (const auto& shader : std::ranges::views::values(inter::HF.shaders))
                inter::shader::Destroy(shader.get());
            inter::HF.shaders.clear();
        }

        bool IsRunning(const Ref<Shader>& shader) { return shader->isRunning; }
    }

    namespace inter::shader
    {
        bool Destroy(Shader* shader)
        {
            if (shader->isRunning)
            {
                shader->isRunning = false;
                rendering::DestroyShader(shader->handle);
                shader->handle = nullptr;
                return true;
            }
            return false;
        }
    }
}