#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <regex>
#include <shaderc/shaderc.hpp>
#include "henums.h"
#include "hstrconversion.h"

namespace fs = std::filesystem;

static std::unordered_map<std::string, std::string> includeMap{};

void LoadIncludeMap(const fs::path& rootDir);
std::string ProcessIncludes(const std::string& inputContent);

shaderc::SpvCompilationResult CompileVulkanShader(const std::string& inputPath, const std::string& outputPath,
    shaderc::Compiler& compiler, shaderc::CompileOptions& options, shaderc_shader_kind kind)
{
    std::ifstream file(inputPath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, inputPath.c_str(), options);
    file.close();
    hassert(result.GetCompilationStatus() == shaderc_compilation_status_success, "Shader compile error: %s", result.GetErrorMessage().c_str());
    std::ofstream out(outputPath, std::ios::binary);
    out.write(reinterpret_cast<const char*>(result.cbegin()),
              std::distance(result.cbegin(), result.cend()) * sizeof(uint32_t));

    return result;
}

void ConvertVulkanShader(const std::string& inputPath, const std::string& outputPath,
    shaderc::Compiler& compiler, shaderc::CompileOptions& options)
{
    std::string processed;
    {
        std::ifstream inFile(inputPath);
        hassert(inFile, "Failed to open input: %s", inputPath.c_str());

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
        log_info_s("Processed: %s -> %s", inputPath.c_str(), tempPath.c_str());
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
        default: hassert(false, "Unknown shader type: %s", inputPath.c_str());
    }

    CompileVulkanShader(tempPath, outputPath, compiler, options, kind);
    fs::remove(tempPath);

    log_info_s("Compiled: %s -> %s", tempPath.c_str(), outputPath.c_str());
}

int main(int argc, char* argv[])
{
    hassert(argc > 3 && (argc - 1) % 2 == 0, "Usage: %s <input1> <output1> [<input2> <output2> ...]", argv[0])

    auto includePath = fs::current_path().parent_path() / fs::path("tools/shaderconvertor/includes/");
    LoadIncludeMap(includePath);

    shaderc::Compiler compiler{};
    shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    for (int i = 1; i < argc; i += 2)
    {
        fs::path inputPath = argv[i];
        fs::path outputPath = argv[i + 1];
        ConvertVulkanShader(inputPath.string(), outputPath.string(), compiler, options);
    }
}

void LoadIncludeMap(const fs::path& rootDir)
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

std::string ProcessIncludes(const std::string& inputContent)
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