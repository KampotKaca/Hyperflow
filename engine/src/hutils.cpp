#include "hyperflow.h"
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

    bool ReadFile(const std::string& filename, bool addNullTerminator, std::vector<char>& result)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            LOG_ERROR("Unable to open file at location %s", filename.c_str());
            return false;
        }
        size_t fileSize = file.tellg();
        uint64_t bufferSize = fileSize + addNullTerminator;
        result = std::vector<char>(bufferSize);
        file.seekg(0);
        file.read(result.data(), (int64_t)fileSize);
        file.close();

        if (addNullTerminator) result[bufferSize - 1] = '\0';

        return true;
    }

    bool FileExists(const char* path)
    {
        struct stat buffer{};
        return stat(path, &buffer) == 0;
    }

    uint32_t TrailingZeros64(uint64_t n)
    {
        return n ? __builtin_ctzll(n) : 64u;
    }
}