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

    bool ReadFile(const std::string& filename, std::vector<char>& result)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Unable to open file at location %s", filename.c_str());
            return false;
        }
        const size_t fileSize = file.tellg();
        result = std::vector<char>(fileSize);
        file.seekg(0);
        file.read(result.data(), (int64_t)fileSize);
        file.close();

        return true;
    }

    bool WriteFile(const std::string& filename, const std::vector<char>& data)
    {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Unable to open file at location %s", filename.c_str());
            return false;
        }
        file.write(data.data(), data.size());
        file.close();
        return true;
    }

    bool FileExists(const char* path)
    {
        struct stat buffer{};
        return stat(path, &buffer) == 0;
    }
}
