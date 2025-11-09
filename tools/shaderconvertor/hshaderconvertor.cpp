#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <regex>

namespace fs = std::filesystem;

static std::unordered_map<std::string, std::string> includeMap{};

void LoadIncludeMap(const fs::path& rootDir);
std::string ProcessIncludes(const std::string& inputContent);

bool CompileVulkanShader(const std::string& inputPath, const std::string& outputPath)
{
    std::string command = "glslc " + inputPath + " -o " + outputPath;
    return std::system(command.c_str()) == 0;
}

void ConvertVulkanShader(const std::string& inputPath, const std::string& outputPath)
{
    std::string processed;
    {
        std::ifstream inFile(inputPath);
        if (!inFile)
        {
            std::cerr << "Failed to open input: " << inputPath << "\n";
            return;
        }

        std::ostringstream buffer;
        buffer << inFile.rdbuf();
        processed = ProcessIncludes(buffer.str());
    }

    std::string tempPath = outputPath;
    if (tempPath.ends_with(".spv"))
        tempPath = tempPath.substr(0, tempPath.length() - 4);

    {
        std::ofstream outFile(tempPath);
        if (!outFile)
        {
            std::cerr << "Failed to write output: " << tempPath << "\n";
            return;
        }

        outFile << processed;
        std::cout << "Processed: " << inputPath << " -> " << tempPath << "\n";
    }

    if (!CompileVulkanShader(tempPath, outputPath))
    {
        std::cerr << "Failed to compile shader: " << tempPath << "\n";
        fs::remove(tempPath);
        return;
    }

    fs::remove(tempPath);
    std::cout << "Compiled: " << tempPath << " -> " << outputPath << "\n";

}

int main(int argc, char* argv[])
{
    if (argc < 3 || (argc - 1) % 2 != 0)
    {
        std::cerr << "Usage: " << argv[0] << " <input1> <output1> [<input2> <output2> ...]\n";
        return 1;
    }

    auto includePath = fs::current_path().parent_path() / fs::path("tools/shaderconvertor/includes/");
    LoadIncludeMap(includePath);
    for (int i = 1; i < argc; i += 2)
    {
        fs::path inputPath = argv[i];
        fs::path outputPath = argv[i + 1];
        ConvertVulkanShader(inputPath.string(), outputPath.string());
    }
}

void LoadIncludeMap(const fs::path& rootDir)
{
    std::cout << "Include Path: " << rootDir << "\n";
    std::regex nameRegex(R"(#name\s+(\w+))");

    for (const auto& entry : fs::recursive_directory_iterator(rootDir))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".glsl")
            continue;

        std::ifstream file(entry.path());
        if (!file)
        {
            std::cerr << "Failed to open: " << entry.path() << "\n";
            continue;
        }

        std::string line;
        std::ostringstream fullSource;
        std::string nameKey;
        bool nameFound = false;

        while (std::getline(file, line))
        {
            std::smatch match;
            if (!nameFound && std::regex_match(line, match, nameRegex))
            {
                nameKey = match[1];
                nameFound = true;
                continue; // Skip this line
            }
            fullSource << line << '\n';
        }

        if (nameFound) includeMap[std::move(nameKey)] = std::move(fullSource).str();
        else std::cerr << "Warning: No '#name' directive found in " << entry.path() << "\n";
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
                std::cerr << "Warning: No include found for key '" << includeKey << "'\n";
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