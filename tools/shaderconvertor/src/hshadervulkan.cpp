//
// Created by ratib on 14.11.2025.
//

#include "hshaderconvertor.h"
#include <regex>
#include <shaderc/shaderc.hpp>

namespace sh
{
    static std::unordered_map<std::string, std::string> includeMap{};

    class GLSL_Includer final : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        shaderc_include_result* GetInclude(
            const char* requested_source,
            shaderc_include_type type,
            const char* requesting_source,
            size_t include_depth) override
        {
            auto it = includeMap.find(requested_source);
            if (it == includeMap.end())
                return MakeResult(requested_source, "", "Include not found");

            return MakeResult(requested_source, it->second, nullptr);
        }

        void ReleaseInclude(shaderc_include_result* data) override
        {
            delete[] data->source_name;
            delete[] data->content;
            delete data;
        }

    private:
        shaderc_include_result* MakeResult(const std::string& name,
                                           const std::string& content,
                                           const char* error_msg)
        {
            auto* r = new shaderc_include_result();
            r->source_name = Clone(name);
            r->source_name_length = name.size();
            r->content = Clone(content);
            r->content_length = content.size();
            r->user_data = nullptr;
            return r;
        }

        char* Clone(const std::string& s)
        {
            char* mem = new char[s.size() + 1];
            memcpy(mem, s.data(), s.size());
            mem[s.size()] = '\0';
            return mem;
        }
    };

    static shaderc::SpvCompilationResult CompileVulkanShader(const std::string& input, const char* inputPath, const char* outputPath, shaderc::Compiler& compiler, shaderc::CompileOptions& options, shaderc_shader_kind kind)
    {
        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(input, kind, inputPath, options);

        hassert(result.GetCompilationStatus() == shaderc_compilation_status_success, "[Hyperflow] Shader compilation error: %s", result.GetErrorMessage().c_str());
        std::ofstream out(outputPath, std::ios::binary);
        out.write((const char*)(result.cbegin()), std::distance(result.cbegin(), result.cend()) * sizeof(uint32_t));

        return result;
    }

    static std::string PreprocessShader(const char* inputPath, shaderc::Compiler& compiler, shaderc::CompileOptions& options)
    {
        std::string source;
        {
            std::ifstream in(inputPath);
            hassert(in, "[Hyperflow] Failed to open input file to preprocess: %s", inputPath);
            std::stringstream ss;
            ss << in.rdbuf();
            source = ss.str();
        }

        shaderc::PreprocessedSourceCompilationResult pre = compiler.PreprocessGlsl(source, shaderc_glsl_infer_from_source, inputPath, options);
        hassert(pre.GetCompilationStatus() == shaderc_compilation_status_success, "[Hyperflow] Shader Preprocessing failed: %s", pre.GetErrorMessage().c_str())

        return { pre.cbegin(), pre.cend() };
    }

    static void ConvertVulkanShader(const char* inputPath, const char* outputPath, shaderc::Compiler& compiler, shaderc::CompileOptions& options)
    {
        std::string processed = PreprocessShader(inputPath, compiler, options);

        shaderc_shader_kind kind;
        auto ext = fs::path(inputPath).extension();
        switch (hf::fnv1a(ext.string()))
        {
            case hf::fnv1a(".vert"): kind = shaderc_vertex_shader; break;
            case hf::fnv1a(".frag"): kind = shaderc_fragment_shader; break;
            case hf::fnv1a(".comp"): kind = shaderc_compute_shader; break;
            case hf::fnv1a(".geom"): kind = shaderc_geometry_shader; break;
            case hf::fnv1a(".tesc"): kind = shaderc_tess_control_shader; break;
            case hf::fnv1a(".tese"): kind = shaderc_tess_evaluation_shader; break;
            default: hassert(false, "Unknown shader type: %s", inputPath);
        }

        CompileVulkanShader(processed, inputPath, outputPath, compiler, options, kind);

        log_info_s("Compiled: %s -> %s", fs::path(inputPath).filename().string().c_str(), fs::path(outputPath).filename().string().c_str());
    }

    static void LoadIncludeMap(const std::vector<std::string>& rootDirs)
    {
        for (const auto& dir : rootDirs)
        {
            if (!fs::exists(dir)) continue;
            for (const auto& entry : fs::recursive_directory_iterator(dir))
            {
                if (!entry.is_regular_file() || entry.path().extension() != ".glsl") continue;

                std::ifstream file(entry.path());
                hassert(file, "Failed to open: %s", entry.path().c_str());

                std::ostringstream fullSource;
                fullSource << file.rdbuf();

                std::string line;
                std::string name = entry.path().filename().string();
                includeMap[name] = fullSource.str();
                file.close();
            }
        }
    }

    void HandleVulkanShaders(const std::vector<std::string>& rootDirs, char** paths, uint32_t count)
    {
        LoadIncludeMap(rootDirs);

        shaderc::Compiler compiler{};
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        options.SetIncluder(std::make_unique<GLSL_Includer>());

        for (int i = 0; i < count; i++) ConvertVulkanShader(paths[i * 2], paths[i * 2 + 1], compiler, options);

    }
}