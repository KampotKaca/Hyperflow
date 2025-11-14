
#include "hshaderconvertor.h"

int main(int argc, char* argv[])
{
    hassert(argc >= 2 && (argc - 2) % 2 == 0, "Incorrect Arguments: <renderer!> <input1> <output1> <input2> <output2>", argv[0])

    if (argc < 4)
    {
        log_warn_s("[Hyperflow] Terminating shader conversion process, [Renderer: %s] reason: No shaders passed!", argv[1]);
        return 0;
    }

    hf::RenderingApiType shaderType = hf::STRING_TO_RENDERING_API_TYPE(argv[1]);
    const auto includePath = std::filesystem::current_path().parent_path() / std::filesystem::path("tools/shaderconvertor/includes/");

    switch (shaderType)
    {
    case hf::RenderingApiType::None:     break;
    case hf::RenderingApiType::Vulkan:   sh::HandleVulkanShaders(includePath, &argv[2], (argc - 2) / 2); break;
    case hf::RenderingApiType::Direct3D: sh::HandleDirect3DShaders(includePath, &argv[2], (argc - 2) / 2); break;
    }
}