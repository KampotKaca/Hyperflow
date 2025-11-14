//
// Created by ratib on 14.11.2025.
//

#include "hshaderconvertor.h"
#include <regex>
#include <shaderc/shaderc.hpp>

namespace sh
{
    static std::unordered_map<std::string, std::string> includeMap{};

    static std::string ProcessIncludes(const std::string& inputContent)
    {
        std::istringstream inputStream(inputContent);
        std::ostringstream outputStream;

        std::string line;
        std::regex includeRegex(R"(^\s*#include\s+["<]?(\w+)[">]?)");

        while (std::getline(inputStream, line))
        {
            std::smatch match;
            if (std::regex_match(line, match, includeRegex))
            {
                const std::string& includeKey = match[1];
                auto it = includeMap.find(includeKey);
                if (it != includeMap.end())
                {
                    outputStream << "// BEGIN include <" << includeKey << ">\n";
                    outputStream << it->second;
                    outputStream << "// END include <" << includeKey << ">\n";
                }
                else
                {
                    log_error("Warning: No include found for key '%s'", includeKey.c_str());
                    outputStream << line << '\n';
                }
            }
            else
            {
                outputStream << line << '\n';
            }
        }

        return outputStream.str();
    }

    static shaderc::SpvCompilationResult CompileVulkanShader(const char* inputPath, const char* outputPath, shaderc::Compiler& compiler, shaderc::CompileOptions& options, shaderc_shader_kind kind)
    {
        std::ifstream file(inputPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, inputPath, options);
        file.close();
        hassert(result.GetCompilationStatus() == shaderc_compilation_status_success, "Shader compile error: %s", result.GetErrorMessage().c_str());
        std::ofstream out(outputPath, std::ios::binary);
        out.write((const char*)(result.cbegin()), std::distance(result.cbegin(), result.cend()) * sizeof(uint32_t));

        return result;
    }

    static void ConvertVulkanShader(const char* inputPath, const char* outputPath, shaderc::Compiler& compiler, shaderc::CompileOptions& options)
    {
        std::string processed;
        {
            std::ifstream inFile(inputPath);
            hassert(inFile, "Failed to open input: %s", inputPath);

            std::ostringstream buffer;
            buffer << inFile.rdbuf();
            processed = ProcessIncludes(buffer.str());
        }

        std::string tempPath = outputPath;
        if (tempPath.ends_with(".spv"))
            tempPath = tempPath.substr(0, tempPath.length() - 4);

        {
            std::ofstream outFile(tempPath);
            hassert(outFile, "Failed to write output: %s", tempPath.c_str());

            outFile << processed;
            log_info_s("Processed: %s -> %s", inputPath, tempPath.c_str());
        }

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

        CompileVulkanShader(tempPath.c_str(), outputPath, compiler, options, kind);
        fs::remove(tempPath);

        log_info_s("Compiled: %s -> %s", tempPath.c_str(), outputPath);
    }

    static void LoadIncludeMap(const fs::path& rootDir)
    {
        std::regex nameRegex(R"(#name\s+(\w+))");

        for (const auto& entry : fs::recursive_directory_iterator(rootDir))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".glsl") continue;

            std::ifstream file(entry.path());
            hassert(file, "Failed to open: %s", entry.path().c_str());

            std::string line;
            std::ostringstream fullSource;
            std::string nameKey;
            bool nameFound = false;

            while (std::getline(file, line))
            {
                std::smatch match;
                if (!nameFound && std::regex_search(line, match, nameRegex))
                {
                    nameKey = match[1];
                    nameFound = true;
                    continue; // Skip this line
                }
                fullSource << line << '\n';
            }

            if (nameFound) includeMap[std::move(nameKey)] = std::move(fullSource).str();
            else log_error("Warning: No '#name' directive found in %s", entry.path().c_str());
        }
    }

    static void CompileShaders(char** paths, uint32_t count)
    {
        shaderc::Compiler compiler{};
        shaderc::CompileOptions options;
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        for (int i = 0; i < count; i++) ConvertVulkanShader(paths[i * 2], paths[i * 2 + 1], compiler, options);
    }

    void HandleVulkanShaders(const fs::path& rootDir, char** paths, uint32_t count)
    {
        LoadIncludeMap(rootDir);
        CompileShaders(paths, count);
    }
}