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

    void ReadTextureDetails(void* yamlTree, void* yamlRoot, TextureDetails& result)
    {
        ryml::Tree tree = *((ryml::Tree*)yamlTree);
        ryml::NodeRef root = *((ryml::NodeRef*)yamlRoot);

        {
            const auto v = root["type"].val();
            std::string_view vView{v.str, v.len};
            result.type = STRING_TO_TEXTURE_TYPE(vView);
        }

        {
            const auto v = root["format"].val();
            std::string_view vView{v.str, v.len};
            result.format = STRING_TO_TEXTURE_FORMAT(vView);
        }

        {
            auto aspectFlags = root["aspectFlags"];
            for (ryml::NodeRef fmt : aspectFlags.children())
            {
                auto v = fmt.val();
                std::string_view vView{v.str, v.len};
                result.aspectFlags |= STRING_TO_TEXTURE_ASPECT_FLAGS(vView);
            }
        }

        {
            const auto v = root["tiling"].val();
            std::string_view vView{v.str, v.len};
            result.tiling = STRING_TO_TEXTURE_TILING(vView);
        }

        {
            auto usageFlags = root["usageFlags"];
            for (ryml::NodeRef fmt : usageFlags.children())
            {
                auto v = fmt.val();
                std::string_view vView{v.str, v.len};
                result.usageFlags |= STRING_TO_TEXTURE_USAGE_FLAGS(vView);
            }
        }

        {
            const auto v = root["memoryType"].val();
            std::string_view vView{v.str, v.len};
            result.memoryType = STRING_TO_BUFFER_MEMORY_TYPE(vView);
        }

        {
            const auto v = root["finalLayout"].val();
            std::string_view vView{v.str, v.len};
            result.finalLayout = STRING_TO_TEXTURE_RESULT_LAYOUT_TYPE(vView);
        }
    }
}
