#include "hshader.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../rendering/include/hex_renderer.h"

namespace hf
{
    Shader::Shader(const ShaderCreationInfo& info)
    : layout(info.layout), library(info.library), modules(info.modules)
    {
        inter::rendering::CreateShader_i(this);
    }

    Shader::~Shader()
    {
        inter::rendering::DestroyShader_i(this);
    }

    Ref<Shader> Create(const ShaderCreationInfo& info)
    {
        Ref<Shader> shader = MakeRef<Shader>(info);
        inter::HF.graphicsResources.shaders[(uint64_t)shader.get()] = shader;
        return shader;
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

    void Destroy(const Ref<Shader>& shader)
    {
        if (inter::rendering::DestroyShader_i(shader.get()))
            inter::HF.graphicsResources.shaders.erase((uint64_t)shader.get());
    }

    bool IsLoaded(const Ref<Shader>& shader) { return shader->handle; }

    namespace inter::rendering
    {
        bool CreateShader_i(Shader* shader)
        {
            if (shader->handle) return false;

            ShaderCreationInfo_i creationInfo{};
            creationInfo.layout = shader->layout;
            creationInfo.library = shader->library->handle;
            creationInfo.modules = shader->modules;

            shader->handle = HF.renderingApi.api.CreateShader(creationInfo);

            return true;
        }

        bool DestroyShader_i(Shader* shader)
        {
            if (shader->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.shaders.push_back(shader->handle);
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
}