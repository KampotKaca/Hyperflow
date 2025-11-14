#include "hshader.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "../rendering/include/hex_renderer.h"
#include "hstrconversion.h"
#include "hyaml.h"

namespace hf
{
    Shader::Shader(const ShaderCreationInfo& info) : layout(info.layout), library(info.library), modules(info.modules)
    {
        ir::rdr::ShaderCreationInfo_i creationInfo{};
        creationInfo.layout = layout;
        creationInfo.library = library->handle;
        creationInfo.modules = modules;

        handle = ir::HF.renderingApi.api.CreateShader(creationInfo);
    }

    Shader::~Shader()
    {
        ir::rdr::DestroyShader_i(this);
    }

    Ref<Shader> Create(const ShaderCreationInfo& info)
    {
        Ref<Shader> shader = MakeRef<Shader>(info);
        ir::HF.graphicsResources.shaders[(uint64_t)shader.get()] = shader;
        return shader;
    }

    void Destroy(const Ref<Shader>* pShaders, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto shader = pShaders[i];
            if (ir::rdr::DestroyShader_i(shader.get()))
                ir::HF.graphicsResources.shaders.erase((uint64_t)shader.get());
        }
    }

    void Destroy(const Ref<Shader>& shader)
    {
        if (ir::rdr::DestroyShader_i(shader.get()))
            ir::HF.graphicsResources.shaders.erase((uint64_t)shader.get());
    }

    bool IsLoaded(const Ref<Shader>& shader) { return shader->handle; }

    namespace ir::rdr
    {
        Ref<Shader> CreateShaderAsset_i(const char* assetPath)
        {
            const auto assetLoc = (TO_RES_PATH_P("shaders") / assetPath).string() + ".meta";

            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            ShaderCreationInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            if (!YamlGetIf_ShaderLayout_i(root, "layout", info.layout)) log_error_s("[Hyperflow] Shader '%s' has invalid layout", assetPath);
            if (!YamlGetIf_i(root, "library", info.library)) log_error_s("[Hyperflow] Shader '%s' has invalid library", assetPath);
            if (!YamlGetIf_i(root, "modulesInfo", info.library, info.modules)) log_error_s("[Hyperflow] Shader %s has invalid moduleInfo");

            return MakeRef<Shader>(info);
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