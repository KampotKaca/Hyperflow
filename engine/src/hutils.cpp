#include "hyaml.h"
#include "hyperflow.h"
#include "hinternal.h"
#include "hstrconversion.h"

#include <sstream>
#include <fstream>

namespace hf::utils
{
    ivec3 ConvertVersion(const char* version)
    {
        std::istringstream ss(version);
        std::string token;

        ivec3 v{};
        uint32_t id = 0;
        while (std::getline(ss, token, '.') && id < 3)
        {
            v[(int32_t)id] = std::stoi(token);
            id++;
        }

        return v;
    }

    static void readFile(std::ifstream& file, List<char>& result)
    {
        const size_t fileSize = file.tellg();
        result = List<char>(fileSize);
        file.seekg(0);
        file.read(result.data(), (int64_t)fileSize);
        file.close();
    }

    bool ReadFile(const char* path, List<char>& result)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path);
            return false;
        }
        readFile(file, result);
        return true;
    }

    bool ReadFile(const std::string& path, List<char>& result)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path.c_str());
            return false;
        }
        readFile(file, result);
        return true;
    }
    bool ReadFile(const std::filesystem::path& path, List<char>& result)
    {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path.string().c_str());
            return false;
        }
        readFile(file, result);
        return true;
    }
    bool ReadFile(const std::string_view path, List<char>& result)
    {
        std::ifstream file(std::string(path), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path);
            return false;
        }
        readFile(file, result);
        return true;
    }

    static void writeFile(std::ofstream& file, const List<char>& data)
    {
        file.write(data.data(), data.size());
        file.close();
    }

    bool WriteFile(const char* path, const List<char>& data)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path);
            return false;
        }
        writeFile(file, data);
        return true;
    }

    bool WriteFile(const std::string& path, const List<char>& data)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path.c_str());
            return false;
        }
        writeFile(file, data);
        return true;
    }

    bool WriteFile(const std::filesystem::path& path, const List<char>& data)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path.string().c_str());
            return false;
        }
        writeFile(file, data);
        return true;
    }

    bool WriteFile(const std::string_view path, const List<char>& data)
    {
        std::ofstream file(std::string(path), std::ios::binary);
        if (!file.is_open())
        {
            log_error("Unable to open file at location %s", path);
            return false;
        }
        writeFile(file, data);
        return true;
    }

    bool FileExists(const char* path)
    {
        struct stat buffer{};
        return stat(path, &buffer) == 0;
    }

    bool FileExists(const std::filesystem::path& path)
    {
        struct stat buffer{};
        return stat(path.string().c_str(), &buffer) == 0;
    }
}
