#include "hshaderlibrary.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    ShaderLibrary::ShaderLibrary(const ShaderLibraryCreationInfo& info)
    : name(info.uniqueLibraryName), outputFormats(info.outputFormats)
    {
        vertexInputModules = std::vector<ShaderLibraryVertexInputModuleInfo>(info.vertexInputModuleCount);
        preRasterModules = std::vector<ShaderLibraryPreRasterModuleInfo>(info.preRasterModuleCount);
        fragmentModules = std::vector<ShaderLibraryFragmentModuleInfo>(info.fragmentModuleCount);
        fragmentOutputModules = std::vector<ShaderLibraryFragmentOutputModuleInfo>(info.fragmentOutputModuleCount);

        uint32_t id = 0;

#define MODULE_CPY(inf, count, res)\
        for (uint32_t i = 0; i < count; i++)\
        {\
            auto& module = inf[i];\
            res[i] = std::move(module.module);\
            *module.resultId = id;\
            id++;\
        }

        MODULE_CPY(info.pVertexInputModules, info.vertexInputModuleCount, vertexInputModules)
        MODULE_CPY(info.pPreRasterModules, info.preRasterModuleCount, preRasterModules)
        MODULE_CPY(info.pFragmentModules, info.fragmentModuleCount, fragmentModules)
        MODULE_CPY(info.pFragmentOutputModules, info.fragmentOutputModuleCount, fragmentOutputModules)

        inter::rendering::CreateShaderLibrary_i(this);

#undef MODULE_CPY
    }

    ShaderLibrary::~ShaderLibrary()
    {
        inter::rendering::DestroyShaderLibrary_i(this);
    }

    Ref<ShaderLibrary> Create(const ShaderLibraryCreationInfo& info)
    {
        Ref<ShaderLibrary> lib = MakeRef<ShaderLibrary>(info);
        inter::HF.graphicsResources.shaderLibraries[(uint64_t)lib.get()] = lib;
        return lib;
    }

    void Destroy(const Ref<ShaderLibrary>& lib)
    {
        if (inter::rendering::DestroyShaderLibrary_i(lib.get()))
            inter::HF.graphicsResources.shaderLibraries.erase((uint64_t)lib.get());
    }

    void Destroy(const Ref<ShaderLibrary>* pLibraries, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto lib = pLibraries[i];
            if (inter::rendering::DestroyShaderLibrary_i(lib.get()))
                inter::HF.graphicsResources.shaderLibraries.erase((uint64_t)lib.get());
        }
    }

    bool IsLoaded(const Ref<ShaderLibrary>& lib) { return lib->handle; }

    namespace inter::rendering
    {

#define HANDLE_FILE(modulePath, post, name)\
        {\
            std::string loc;\
            if (modulePath.isAbsolute) loc = modulePath.path;\
            else loc = TO_RES_PATH(std::string("shaders/vulkan/") + modulePath.path) + post;\
            if (!utils::FileExists(loc.c_str()))\
            {\
                LOG_ERROR("[Hyperflow] Unable to find %s shader: %s", name, loc.c_str());\
                return false;\
            }\
            std::vector<char> code{};\
            utils::ReadFile(loc, code);\
            moduleCodes.push_back(std::move(code));\
        }

        bool CreateShaderLibrary_i(ShaderLibrary* lib)
        {
            if (lib->handle) return false;

            auto preRasterModules = std::vector<ShaderLibraryPreRasterModuleInfo_i>(lib->preRasterModules.size());
            auto fragmentModules = std::vector<ShaderLibraryFragmentModuleInfo_i>(lib->fragmentModules.size());

            auto moduleCodes = std::vector<std::vector<char>>();

            for (uint32_t i = 0; i < preRasterModules.size(); i++)
            {
                auto& moduleInfo = lib->preRasterModules[i];
                int32_t vertexCodeIndex = -1, tesselationControlCodeIndex = -1,
                tesselationEvaluationCodeIndex = -1, geometryCodeIndex = -1;

                switch (HF.renderingApi.type)
                {
                case RenderingApiType::Vulkan:
                {
                    HANDLE_FILE(moduleInfo.vertexShaderPath, ".vert.spv", "Vertex")
                    vertexCodeIndex = (int32_t)moduleCodes.size() - 1;

                    if (moduleInfo.tessellationControlShaderPath.has_value())
                    {
                        HANDLE_FILE(moduleInfo.tessellationControlShaderPath.value(), ".tesc.spv", "Tesselation Control")
                        tesselationControlCodeIndex = (int32_t)moduleCodes.size() - 1;
                    }

                    if (moduleInfo.tessellationEvaluationShaderPath.has_value())
                    {
                        HANDLE_FILE(moduleInfo.tessellationEvaluationShaderPath.value(), ".tese.spv", "Tesselation Evaluation")
                        tesselationEvaluationCodeIndex = (int32_t)moduleCodes.size() - 1;
                    }

                    if (moduleInfo.geometryShaderPath.has_value())
                    {
                        HANDLE_FILE(moduleInfo.geometryShaderPath.value(), ".geom.spv", "Geometry")
                        geometryCodeIndex = (int32_t)moduleCodes.size() - 1;
                    }
                }
                break;
                case RenderingApiType::Direct3D:
                    break;
                default:
                    throw GENERIC_EXCEPT("[Hyperflow]", "Cannot create shader without loading renderer");
                }

                ShaderLibraryPreRasterModuleInfo_i info
                {
                    .options = moduleInfo.options,
                    .layout = moduleInfo.layout,
                };

                {
                    auto& v = moduleCodes[(size_t)vertexCodeIndex];
                    info.vertexShaderCode = v.data();
                    info.vertexShaderCodeSize = (uint32_t)v.size();
                }

                if (tesselationControlCodeIndex != -1)
                {
                    auto& v = moduleCodes[(size_t)tesselationControlCodeIndex];
                    info.tessellationControlShaderCode = v.data();
                    info.tessellationControlShaderCodeSize = (uint32_t)v.size();
                }

                if (tesselationEvaluationCodeIndex != -1)
                {
                    auto& v = moduleCodes[(size_t)tesselationEvaluationCodeIndex];
                    info.tessellationEvaluationShaderCode = v.data();
                    info.tessellationEvaluationShaderCodeSize = (uint32_t)v.size();
                }

                if (geometryCodeIndex != -1)
                {
                    auto& v = moduleCodes[(size_t)geometryCodeIndex];
                    info.geometryShaderCode = v.data();
                    info.geometryShaderCodeSize = (uint32_t)v.size();
                }

                preRasterModules[i] = info;
            }

            for (uint32_t i = 0; i < fragmentModules.size(); i++)
            {
                auto& moduleInfo = lib->fragmentModules[i];
                int32_t fragmentCodeIndex = -1;

                switch (HF.renderingApi.type)
                {
                case RenderingApiType::Vulkan:
                {
                    HANDLE_FILE(moduleInfo.fragmentShaderPath, ".frag.spv", "Fragment")
                    fragmentCodeIndex = (int32_t)moduleCodes.size() - 1;
                }
                    break;
                case RenderingApiType::Direct3D:
                    break;
                default:
                    throw GENERIC_EXCEPT("[Hyperflow]", "Cannot create shader without loading renderer");
                }

                ShaderLibraryFragmentModuleInfo_i info
                {
                    .depthStencilOptions = moduleInfo.depthStencilOptions,
                    .layout = moduleInfo.layout,
                };

                {
                    auto& v = moduleCodes[(size_t)fragmentCodeIndex];
                    info.fragmentShaderCode = v.data();
                    info.fragmentShaderCodeSize = (uint32_t)v.size();
                }

                fragmentModules[i] = info;
            }

            ShaderLibraryCreationInfo_i info
            {
                .uniqueLibraryName = lib->name.c_str(),
                .outputFormats = lib->outputFormats,
                .pVertexInputModules = lib->vertexInputModules.data(),
                .vertexInputModuleCount = (uint32_t)lib->vertexInputModules.size(),
                .pPreRasterModules = preRasterModules.data(),
                .preRasterModuleCount = (uint32_t)preRasterModules.size(),
                .pFragmentModules = fragmentModules.data(),
                .fragmentModuleCount = (uint32_t)fragmentModules.size(),
                .pFragmentOutputModules = lib->fragmentOutputModules.data(),
                .fragmentOutputModuleCount = (uint32_t)lib->fragmentOutputModules.size()
            };

            lib->handle = HF.renderingApi.api.CreateShaderLibrary(info);
            return true;
        }

        bool DestroyShaderLibrary_i(ShaderLibrary* lib)
        {
            if (lib->handle)
            {
                std::lock_guard lock(HF.deletedResources.syncLock);
                HF.deletedResources.shaderLibraries.push_back(lib->handle);
                lib->handle = nullptr;
                return true;
            }
            return false;
        }

        void DestroyAllShaderLibraries_i(bool internalOnly)
        {
            for (const auto& lib : std::ranges::views::values(HF.graphicsResources.shaderLibraries))
                DestroyShaderLibrary_i(lib.get());
            if (!internalOnly) HF.graphicsResources.shaderLibraries.clear();
        }
    }
}