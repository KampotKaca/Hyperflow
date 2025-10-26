#include "hyaml.h"
#include "hshaderlibrary.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"
#include "hstrconversion.h"

namespace hf
{
    ShaderLibrary::ShaderLibrary(const ShaderLibraryCreationInfo& info)
    : name(info.cacheFileName)
    {
#define HANDLE_FILE(modulePath, post, name)\
        {\
            std::string loc;\
            if (modulePath.isAbsolute) loc = modulePath.path;\
            else loc = TO_RES_PATH(std::string("shaders/vulkan/") + modulePath.path) + post;\
            if (!utils::FileExists(loc.c_str()))\
            {\
                LOG_ERROR("[Hyperflow] Unable to find %s shader: %s", name, loc.c_str());\
                return;\
            }\
            List<char> code{};\
            utils::ReadFile(loc, code);\
            moduleCodes.push_back(std::move(code));\
        }

        auto preRastModules = std::vector<inter::rendering::ShaderLibraryPreRasterModuleInfo_i>(info.preRasterModuleCount);
        auto fragModules = std::vector<inter::rendering::ShaderLibraryFragmentModuleInfo_i>(info.fragmentModuleCount);

        auto moduleCodes = std::vector<std::vector<char>>();

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

            switch (inter::HF.renderingApi.type)
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
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Cannot create shader without loading renderer");
            }

            inter::rendering::ShaderLibraryPreRasterModuleInfo_i prInfo{};
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

            switch (inter::HF.renderingApi.type)
            {
            case RenderingApiType::Vulkan:
            {
                HANDLE_FILE(moduleInfo.fragmentShaderPath, ".frag.spv", "Fragment")
                fragmentCodeIndex = (int32_t)moduleCodes.size() - 1;
            }
                break;
            case RenderingApiType::Direct3D: break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Cannot create shader without loading renderer");
            }

            inter::rendering::ShaderLibraryFragmentModuleInfo_i fmInfo{};
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

        inter::rendering::ShaderLibraryCreationInfo_i slInfo{};
        slInfo.uniqueLibraryName         = name.c_str();
        slInfo.outputFormats             = inter::HF.internalResourcesFormat.drawOutputFormats;
        slInfo.pVertexInputModules       = info.pVertexInputModules;
        slInfo.vertexInputModuleCount    = (uint32_t)info.vertexInputModuleCount;
        slInfo.pPreRasterModules         = preRastModules.data();
        slInfo.preRasterModuleCount      = (uint32_t)preRastModules.size();
        slInfo.pFragmentModules          = fragModules.data();
        slInfo.fragmentModuleCount       = (uint32_t)fragModules.size();
        slInfo.pFragmentOutputModules    = info.pFragmentOutputModules;
        slInfo.fragmentOutputModuleCount = (uint32_t)info.fragmentOutputModuleCount;

        handle = inter::HF.renderingApi.api.CreateShaderLibrary(slInfo);
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

    uint32_t GetVertexInputModule   (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->vertexInputModules[name]; }
    uint32_t GetVertexInputModule   (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->vertexInputModules[name]; }
    uint32_t GetPreRasterModule     (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->preRasterModules[name]; }
    uint32_t GetPreRasterModule     (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->preRasterModules[name]; }
    uint32_t GetFragmentModule      (const Ref<ShaderLibrary>& lib, const char* name)            { return lib->fragmentModules[name]; }
    uint32_t GetFragmentModule      (const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->fragmentModules[name]; }
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const char* name)            { return lib->fragmentOutputModules[name]; }
    uint32_t GetFragmentOutputModule(const Ref<ShaderLibrary>& lib, const std::string_view name) { return lib->fragmentOutputModules[name]; }

    namespace inter::rendering
    {
        static void ReadVertexInputModule   (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryVertexInputModuleInfo& result);
        static void ReadPreRasterModule     (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryPreRasterModuleInfo& result);
        static void ReadFragmentModule      (const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentModuleInfo& result);
        static void ReadFragmentOutputModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentOutputModuleInfo& result);

        Ref<ShaderLibrary> CreateShaderLibraryAsset_i(const char* assetPath)
        {
            const auto assetLoc = (TO_RES_PATH_P("shadermodules") / assetPath).string() + ".meta";
            const auto moduleFolder = std::filesystem::path(assetLoc).parent_path();

            std::vector<char> metadata{};
            if (!START_READING(assetLoc.c_str(), metadata)) return nullptr;

            try
            {
                ShaderLibraryCreationInfo info{};

                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["cacheFileName"].val();
                    const std::string_view vView{v.str, v.len};
                    info.cacheFileName = vView;
                }

                const auto vertexInputModuleFolder = moduleFolder / "vertexinputs";
                const auto preRasterModuleFolder = moduleFolder / "prerasters";
                const auto fragmentModuleFolder = moduleFolder / "fragments";
                const auto fragmentOutputModuleFolder = moduleFolder / "fragmentoutputs";

                std::vector<ShaderLibraryVertexInputModuleInfo> vertexInputModules{};
                std::vector<ShaderLibraryPreRasterModuleInfo> preRasterModules{};
                std::vector<ShaderLibraryFragmentModuleInfo> fragmentModules{};
                std::vector<ShaderLibraryFragmentOutputModuleInfo> fragmentOutputModules{};

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
            catch(const HyperException& e)
            {
                log_error(e.GetFile().c_str(), e.GetLine(), e.what());
            }
            catch(const std::exception& e)
            {
                LOG_ERROR("[Hyperflow] Error parsing Shader Library: %s\nError: %s", assetPath, e.what());
            }
            catch (...)
            {
                LOG_ERROR("[Hyperflow] Unknown error parsing Shader Library: %s", assetPath);
            }
            return nullptr;
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
            std::vector<char> metadata{};
            if(!START_READING(assetPath, metadata)) return;

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();
            try
            {
                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["enablePrimitiveRestart"].val();
                    const std::string_view vView{v.str, v.len};
                    result.enablePrimitiveRestart = ConvertToBool_i(vView);
                }

                {
                    const auto v = root["topology"].val();
                    const std::string_view vView{v.str, v.len};
                    result.topology = STRING_TO_PRIMITIVE_TOPOLOGY_TYPE(vView);
                }

                {
                    const auto attributes = root["attributes"];
                    result.attributeCount = attributes.num_children();
                    uint32_t index = 0;
                    for (auto fmt : attributes.children())
                    {
                        const auto v = fmt.val();
                        const std::string_view vView{v.str, v.len};
                        result.pAttributes[index] = FindVertexAttribute(std::string(vView).c_str());
                        index++;
                    }
                }

            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Vertex Input Module: %s", (const char*)assetPath.c_str());
            }
        }

        void ReadPreRasterModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryPreRasterModuleInfo& result)
        {
            std::vector<char> metadata{};
            if(!START_READING(assetPath, metadata)) return;

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();
            try
            {
                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["layout"].val();
                    const std::string_view vView{v.str, v.len};
                    result.layout = FindShaderLayout(vView);
                }

                {
                    const auto v = root["vertexShaderPath"].val();
                    const std::string_view vView{v.str, v.len};
                    result.vertexShaderPath.path = std::string(vView);
                }

                if (root.has_child("tessellationControlShaderPath"))
                {
                    auto node = root["tessellationControlShaderPath"];
                    if (node.has_val())
                    {
                        const auto v = node.val();
                        const std::string_view vView{v.str, v.len};
                        result.tessellationControlShaderPath = FilePath{ .path = std::string(vView) };
                    }
                }

                if (root.has_child("tessellationEvaluationShaderPath"))
                {
                    auto node = root["tessellationEvaluationShaderPath"];
                    if (node.has_val())
                    {
                        const auto v = node.val();
                        const std::string_view vView{v.str, v.len};
                        result.tessellationEvaluationShaderPath = FilePath{ .path = std::string(vView) };
                    }
                }

                if (root.has_child("geometryShaderPath"))
                {
                    auto node = root["geometryShaderPath"];
                    if (node.has_val())
                    {
                        const auto v = node.val();
                        const std::string_view vView{v.str, v.len};
                        result.geometryShaderPath = FilePath{ .path = std::string(vView) };
                    }
                }

                ReadRasterizerOptions_i(root["options"], result.options);

            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Pre Raster Module: %s", (const char*)assetPath.c_str());
            }
        }

        void ReadFragmentModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentModuleInfo& result)
        {
            std::vector<char> metadata{};
            if(!START_READING(assetPath, metadata)) return;

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();
            try
            {
                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    const auto v = root["layout"].val();
                    const std::string_view vView{v.str, v.len};
                    result.layout = FindShaderLayout(vView);
                }

                {
                    const auto v = root["fragmentShaderPath"].val();
                    const std::string_view vView{v.str, v.len};
                    result.fragmentShaderPath.path = std::string(vView);
                }

                ReadDepthStencilOptions_i(root["depthStencilOptions"], result.depthStencilOptions);

            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Fragment Module: %s", (const char*)assetPath.c_str());
            }
        }

        void ReadFragmentOutputModule(const std::filesystem::path& assetPath, const std::filesystem::path& parentFolderPath, ShaderLibraryFragmentOutputModuleInfo& result)
        {
            std::vector<char> metadata{};
            if(!START_READING(assetPath, metadata)) return;

            result.name = std::filesystem::relative(assetPath, parentFolderPath).replace_extension("").string();
            try
            {
                ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(metadata.data()));
                ryml::NodeRef root = tree.rootref();

                {
                    if (root.has_child("blendOp"))
                    {
                        auto node = root["blendOp"];
                        if (node.has_val())
                        {
                            const auto v = node.val();
                            const std::string_view vView{v.str, v.len};
                            result.blendOp = STRING_TO_SHADER_BLEND_OPERATION(vView);
                        }
                    }

                    const auto colorAttachmentsSettings = root["colorAttachmentsSettings"];
                    result.colorAttachmentCount = colorAttachmentsSettings.num_children();
                    uint32_t index = 0;
                    for (auto colorAttachmentSetting : colorAttachmentsSettings.children())
                    {
                        ColorAttachmentSettings colorAttachment{};

                        {
                            const auto v = colorAttachmentSetting["colorWriteMask"].val();
                            const std::string_view vView{v.str, v.len};
                            colorAttachment.colorWriteMask = STRING_TO_COLOR_MASKING_FLAGS(vView);
                        }

                        if (colorAttachmentSetting.has_child("blendingOptions"))
                        {
                            ShaderBlendingOptions blendingOptions{};
                            ReadShaderBlendingOptions_i(colorAttachmentSetting["blendingOptions"], blendingOptions);
                            colorAttachment.blendingOptions = blendingOptions;
                        }

                        result.pColorAttachmentsSettings[index] = colorAttachment;
                        index++;
                    }
                }
            }catch (...)
            {
                LOG_ERROR("[Hyperflow] Error parsing Fragment Output Module: %s", (const char*)assetPath.c_str());
            }
        }

        //endregion
    }
}