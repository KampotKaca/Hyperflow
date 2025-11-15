
#include "hshaderconvertor.h"

int main(int argc, char* argv[])
{
    constexpr uint32_t HEADER_SIZE = 3;
    hassert(argc >= HEADER_SIZE, "Incorrect Arguments: <renderer!> <include paths> <input1> <output1> <input2> <output2> %s", argv[0])
    uint32_t headerCount = std::stoi(argv[2]);
    uint32_t headerSize = HEADER_SIZE + headerCount;

    hassert((argc - headerSize) % 2 == 0, "Incorrect Arguments: <renderer!> <include paths> <input1> <output1> <input2> <output2> %i", headerCount)

    if (argc < headerSize + 2)
    {
        log_warn_s("[Hyperflow] Terminating shader conversion process, [Renderer: %s] reason: No shaders passed!", argv[1]);
        return 0;
    }

    hf::RenderingApiType shaderType = hf::STRING_TO_RENDERING_API_TYPE(argv[1]);
    const auto includePath = std::filesystem::current_path().parent_path() / std::filesystem::path("tools/shaderconvertor/includes/");

    std::vector<std::string> result;
    for (uint32_t i = 0; i < headerCount; i++) result.emplace_back(argv[HEADER_SIZE + i]);

    switch (shaderType)
    {
    case hf::RenderingApiType::None:     break;
    case hf::RenderingApiType::Vulkan:   sh::HandleVulkanShaders(result, &argv[headerSize], (argc - headerSize) / 2); break;
    case hf::RenderingApiType::Direct3D: sh::HandleDirect3DShaders(result, &argv[headerSize], (argc - headerSize) / 2); break;
    }
}