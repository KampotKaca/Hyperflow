#include "hshader.h"
#include "hshared.h"
#include "hinternal.h"
#include "hyperflow.h"

namespace hf
{
    Shader::Shader(const std::string& vertPath, const std::string& fragPath)
    {
        vPath = vertPath;
        fPath = fragPath;
        std::vector<char> vertexCode{}, fragmentCode{};
        utils::ReadFile(TO_RES_PATH(vertPath), vertexCode);
        utils::ReadFile(TO_RES_PATH(fragPath), fragmentCode);

        isRunning = true;
    }

    Shader::~Shader()
    {
        inter::rendering::DestroyShader(handle);
        handle = nullptr;
    }

    namespace shader
    {
        bool IsRunning(const Ref<Shader>& shader) { return shader->isRunning; }
    }
}