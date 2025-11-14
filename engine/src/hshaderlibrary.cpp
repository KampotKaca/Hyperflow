#include "hyaml.h"
#include "hshaderlibrary.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hstrconversion.h"

namespace hf
{
    ShaderLibrary::ShaderLibrary(const ShaderLibraryCreationInfo& info) : name(info.cacheFileName)
    {
#define HANDLE_FILE(modulePath, post, name)\
        {\
            std::string loc = TO_RES_PATH(std::string("shaders/vulkan/") + modulePath) + post;\
            if (!utils::FileExists(loc.c_str()))\
            {\
                log_error("[Hyperflow] Unable to find %s shader: %s", name, loc.c_str());\
                return;\
            }\
            List<char> code{};\
            utils::ReadFile(loc, code);\
            moduleCodes.push_back(std::move(code));\
        }

        auto preRastModules = List<ir::rdr::ShaderLibraryPreRasterModuleInfo_i>(info.preRasterModuleCount);
        auto fragModules = List<ir::rdr::ShaderLibraryFragmentModuleInfo_i>(info.fragmentModuleCount);

        auto moduleCodes = List<List<char>>();

        uint32_t moduleId = 0;
        for (uint32_t i = 0; i < info.vertexInputModuleCount; i++)
        {
            vertexInputModules[info.pVertexInputModules[i].name] = moduleId;
            moduleId++;
        }

        for (uint32_t i = 0; i < info.preRasterModuleCount; i++)
        {
            auto& moduleInfo = info.pPreRasterModules[i];
            int32_t vertexCodeIndex = -1, tesselationControlCodeIndex = -1,
            tesselationEvaluationCodeIndex = -1, geometryCodeIndex = -1;

            switch (ir::HF.renderingApi.type)
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
            case RenderingApiType::Direct3D: break;
            default:
                log_fatal("[Hyperflow] Cannot create shader without loading renderer");
                abort();
            }

            ir::rdr::ShaderLibraryPreRasterModuleInfo_i prInfo{};
            prInfo.options = moduleInfo.options;
            prInfo.layout = moduleInfo.layout;

            {
                auto& v = moduleCodes[(size_t)vertexCodeIndex];
                prInfo.vertexShaderCode = v.data();
                prInfo.vertexShaderCodeSize = (uint32_t)v.size();
            }

                if (tesselationControlCodeIndex != -1)
            {
                auto& v = moduleCodes[(size_t)tesselationControlCodeIndex];
                prInfo.tessellationControlShaderCode = v.data();
                prInfo.tessellationControlShaderCodeSize = (uint32_t)v.size();
            }

            if (tesselationEvaluationCodeIndex != -1)
            {
                auto& v = moduleCodes[(size_t)tesselationEvaluationCodeIndex];
                prInfo.tessellationEvaluationShaderCode = v.data();
                prInfo.tessellationEvaluationShaderCodeSize = (uint32_t)v.size();
            }

            if (geometryCodeIndex != -1)
            {
                auto& v = moduleCodes[(size_t)geometryCodeIndex];
                prInfo.geometryShaderCode = v.data();
                prInfo.geometryShaderCodeSize = (uint32_t)v.size();
            }

            preRastModules[i] = prInfo;
            preRasterModules[moduleInfo.name] = moduleId;
            moduleId++;
        }

        for (uint32_t i = 0; i < info.fragmentModuleCount; i++)
        {
            auto& moduleInfo = info.pFragmentModules[i];
            int32_t fragmentCodeIndex = -1;

            switch (ir::HF.renderingApi.type)
            {
            case RenderingApiType::Vulkan:
            {
                HANDLE_FILE(moduleInfo.fragmentShaderPath, ".frag.spv", "Fragment")
                fragmentCodeIndex = (int32_t)moduleCodes.size() - 1;
            }
                break;
            case RenderingApiType::Direct3D: break;
            default:
                log_fatal("[Hyperflow] Cannot create shader without loading renderer");
                abort();
            }

            ir::rdr::ShaderLibraryFragmentModuleInfo_i fmInfo{};
            fmInfo.depthStencilOptions = moduleInfo.depthStencilOptions;
            fmInfo.layout = moduleInfo.layout;

            {
                auto& v = moduleCodes[(size_t)fragmentCodeIndex];
                fmInfo.fragmentShaderCode = v.data();
                fmInfo.fragmentShaderCodeSize = (uint32_t)v.size();
            }

            fragModules[i] = fmInfo;
            fragmentModules[moduleInfo.name] = moduleId;
            moduleId++;
        }

        for (uint32_t i = 0; i < info.fragmentOutputModuleCount; i++)
        {
            fragmentOutputModules[info.pFragmentOutputModules[i].name] = moduleId;
            moduleId++;
        }

        ir::rdr::ShaderLibraryCreationInfo_i slInfo{};
        slInfo.uniqueLibraryName         = name.c_str();
        slInfo.outputFormats             = ir::HF.internalResourcesFormat.drawOutputFormats;
        slInfo.pVertexInputModules       = info.pVertexInputModules;
        slInfo.vertexInputModuleCount    = (uint32_t)info.vertexInputModuleCount;
        slInfo.pPreRasterModules         = preRastModules.data();
        slInfo.preRasterModuleCount      = (uint32_t)preRastModules.size();
        slInfo.pFragmentModules          = fragModules.data();
        slInfo.fragmentModuleCount       = (uint32_t)fragModules.size();
        slInfo.pFragmentOutputModules    = info.pFragmentOutputModules;
        slInfo.fragmentOutputModuleCount = (uint32_t)info.fragmentOutputModuleCount;

        handle = ir::HF.renderingApi.api.CreateShaderLibrary(slInfo);
    }

    ShaderLibrary::~ShaderLibrary()
    {
        ir::rdr::DestroyShaderLibrary_i(this);
    }

    Ref<ShaderLibrary> Create(const ShaderLibraryCreationInfo& info)
    {
        Ref<ShaderLibrary> lib = MakeRef<ShaderLibrary>(info);
        ir::HF.graphicsResources.shaderLibraries[(uint64_t)lib.get()] = lib;
        return lib;
    }

    void Destroy(const Ref<ShaderLibrary>& lib)
    {
        if (ir::rdr::DestroyShaderLibrary_i(lib.get()))
            ir::HF.graphicsResources.shaderLibraries.erase((uint64_t)lib.get());
    }

    void Destroy(const Ref<ShaderLibrary>* pLibraries, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto lib = pLibraries[i];
            if (ir::rdr::DestroyShaderLibrary_i(lib.get()))
                ir::HF.graphicsResources.shaderLibraries.erase((uint64_t)lib.get());
        }
    }

    bool IsLoaded(const Ref<ShaderLibrary>& lib) { return lib->handle; }

    uint32_t GetVertexInputModule   (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->vertexInputModules[name]; }
    uint32_t GetVertexInputModule   (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->vertexInputModules[name]; }
    uint32_t GetPreRasterModule     (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->preRasterModules[name]; }
    uint32_t GetPreRasterModule     (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->preRasterModules[name]; }
    uint32_t GetFragmentModule      (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->fragmentModules[name]; }
    uint32_t GetFragmentModule      (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->fragmentModules[name]; }
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const char* name)            { return lib->fragmentOutputModules[name]; }
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->fragmentOutputModules[name]; }

    namespace ir::rdr
    {
        static void ReadVertexInputModule   (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryVertexInputModuleInfo& result);
        static void ReadPreRasterModule     (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryPreRasterModuleInfo& result);
        static void ReadFragmentModule      (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentModuleInfo& result);
        static void ReadFragmentOutputModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentOutputModuleInfo& result);

        Ref<ShaderLibrary> CreateShaderLibraryAsset_i(const char* assetPath)
        {
            const auto assetLoc = (TO_RES_PATH_P("shadermodules") / assetPath).string() + ".meta";
            const auto moduleFolder = std::filesystem::path(assetLoc).parent_path();

            List<char> metadata{};
            START_READING(assetLoc.c_str(), metadata);

            ShaderLibraryCreationInfo info{};

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            if(!root.get_if("cacheFileName", &info.cacheFileName)) log_warn_s("[Hyperflow] Shader library '%s' does not have a cache file name", assetPath);

            const auto vertexInputModuleFolder = moduleFolder / "vertexinputs";
            const auto preRasterModuleFolder = moduleFolder / "prerasters";
            const auto fragmentModuleFolder = moduleFolder / "fragments";
            const auto fragmentOutputModuleFolder = moduleFolder / "fragmentoutputs";

            List<ShaderLibraryVertexInputModuleInfo> vertexInputModules{};
            List<ShaderLibraryPreRasterModuleInfo> preRasterModules{};
            List<ShaderLibraryFragmentModuleInfo> fragmentModules{};
            List<ShaderLibraryFragmentOutputModuleInfo> fragmentOutputModules{};

            for (const auto& entry : std::filesystem::recursive_directory_iterator(vertexInputModuleFolder))
            {
                if (entry.is_regular_file())
                {
                    ShaderLibraryVertexInputModuleInfo moduleInfo{};
                    ReadVertexInputModule(entry.path(), vertexInputModuleFolder.c_str(), moduleInfo);
                    vertexInputModules.push_back(moduleInfo);
                }
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(preRasterModuleFolder))
            {
                if (entry.is_regular_file())
                {
                    ShaderLibraryPreRasterModuleInfo moduleInfo{};
                    ReadPreRasterModule(entry.path(), preRasterModuleFolder.c_str(), moduleInfo);
                    preRasterModules.push_back(moduleInfo);
                }
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(fragmentModuleFolder))
            {
                if (entry.is_regular_file())
                {
                    ShaderLibraryFragmentModuleInfo moduleInfo{};
                    ReadFragmentModule(entry.path(), fragmentModuleFolder.c_str(), moduleInfo);
                    fragmentModules.push_back(moduleInfo);
                }
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(fragmentOutputModuleFolder))
            {
                if (entry.is_regular_file())
                {
                    ShaderLibraryFragmentOutputModuleInfo moduleInfo{};
                    ReadFragmentOutputModule(entry.path(), fragmentOutputModuleFolder.c_str(), moduleInfo);
                    fragmentOutputModules.push_back(moduleInfo);
                }
            }

            info.pVertexInputModules       = vertexInputModules.data();
            info.vertexInputModuleCount    = vertexInputModules.size();
            info.pPreRasterModules         = preRasterModules.data();
            info.preRasterModuleCount      = preRasterModules.size();
            info.pFragmentModules          = fragmentModules.data();
            info.fragmentModuleCount       = fragmentModules.size();
            info.pFragmentOutputModules    = fragmentOutputModules.data();
            info.fragmentOutputModuleCount = fragmentOutputModules.size();

            return MakeRef<ShaderLibrary>(info);
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

        //region Modules

        void ReadVertexInputModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryVertexInputModuleInfo& result)
        {
            List<char> metadata{};
            START_READING(assetPath, metadata);

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            if (!YamlGetIf_i(root, "enablePrimitiveRestart", result.enablePrimitiveRestart)) log_warn_s("[Hyperflow] Vertex input module '%s' does not have a enablePrimitiveRestart", assetPath.string().c_str());
            if (!YamlGetIf_i(root, "topology", result.topology)) log_warn_s("[Hyperflow] Vertex input module '%s' does not have a topology", assetPath.string().c_str());

            auto attribCount = YamlGetIfArray_i(root, "attributes", result.pAttributes);
            if (attribCount >= 0) result.attributeCount = (uint32_t)attribCount;
            else
            {
                log_error_s("[Hyperflow] Vertex input module '%s' has invalid attribute", assetPath.string().c_str());
                result.attributeCount = 0;
            }
        }

        void ReadPreRasterModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryPreRasterModuleInfo& result)
        {
            List<char> metadata{};
            START_READING(assetPath, metadata);

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            std::string tessellationControlShaderPath{};
            std::string tessellationEvaluationShaderPath{};
            std::string geometryShaderPath{};

            if (!YamlGetIf_ShaderLayout_i(root, "layout", result.layout)) log_error_s("[Hyperflow] Pre raster module '%s' has invalid layout", assetPath.string().c_str());
            if (!root.get_if("vertexShaderPath", &result.vertexShaderPath)) log_error_s("[Hyperflow] Pre raster module '%s' has invalid vertexShaderPath", assetPath.string().c_str());

            if (root.get_if("tessellationControlShaderPath", &tessellationControlShaderPath)) result.tessellationControlShaderPath = tessellationControlShaderPath;
            if (root.get_if("tessellationEvaluationShaderPath", &tessellationEvaluationShaderPath)) result.tessellationEvaluationShaderPath = tessellationEvaluationShaderPath;
            if (root.get_if("geometryShaderPath", &geometryShaderPath)) result.geometryShaderPath = geometryShaderPath;

            if (!YamlGetIf_i(root, "options", result.options)) log_error_s("[Hyperflow] Pre raster module '%s' has invalid options", assetPath.string().c_str());
        }

        void ReadFragmentModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentModuleInfo& result)
        {
            List<char> metadata{};
            START_READING(assetPath, metadata);

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            if (!YamlGetIf_ShaderLayout_i(root, "layout", result.layout)) log_error_s("[Hyperflow] Fragment module '%s' has invalid layout", assetPath.string().c_str());
            if (!root.get_if("fragmentShaderPath", &result.fragmentShaderPath)) log_error_s("[Hyperflow] Fragment module '%s' has invalid fragmentShaderPath", assetPath.string().c_str());
            if (!YamlGetIf_i(root, "depthStencilOptions", result.depthStencilOptions)) log_error_s("[Hyperflow] Fragment module '%s' has invalid depthStencilOptions", assetPath.string().c_str());
        }

        void ReadFragmentOutputModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentOutputModuleInfo& result)
        {
            List<char> metadata{};
            START_READING(assetPath, metadata);

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();

            ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
            ryml::NodeRef root = tree.rootref();

            {
                ShaderBlendOp blendOp{};
                if (YamlGetIf_i(root, "blendOp", blendOp)) result.blendOp = blendOp;
            }

            int32_t colorAttachmentCount = YamlGetIfArray_i(root, "colorAttachmentsSettings", result.pColorAttachmentsSettings);
            if (colorAttachmentCount >= 0) result.colorAttachmentCount = (uint32_t)colorAttachmentCount;
            else log_error_s("[Hyperflow] Fragment output module '%s' has invalid colorAttachmentsSettings", assetPath.string().c_str());
        }

        //endregion
    }
}