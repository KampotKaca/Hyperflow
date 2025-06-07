#ifndef HPACKETS_H
#define HPACKETS_H

#include "hshared.h"
#include "../config.h"

namespace hf
{
    struct DrawPacketInfo
    {
        uint32_t texpackStart{};
        uint32_t texpackCount{};

        uint32_t drawCallStart{};
        uint32_t drawCallCount{};

        uint8_t pushConstantBuffer[RENDERING_MAX_PUSH_CONSTANT_SIZE + 1];
    };

    struct MaterialPacketInfo
    {
        Ref<Material> material{};
        uint32_t drawPacketStart{};
        uint32_t drawPacketCount{};
    };

    struct ShaderPacketInfo
    {
        ShaderBindingInfo bindingInfo{};
        uint32_t materialPacketStart{};
        uint32_t materialPacketCount{};
    };

    struct ShaderSetupPacketInfo
    {
        ShaderSetup shaderSetup{};
        uint16_t shaderStart{};
        uint16_t shaderCount{};

        uint32_t uniformStart{};
        uint32_t uniformCount{};

        uint32_t texpackStart{};
        uint32_t texpackCount{};
    };

    struct RenderPassPacketInfo
    {
        RenderPass pass{};
        uint16_t shaderSetupStart{};
        uint16_t shaderSetupCount{};
    };

    struct RenderPacket
    {
        RenderPassPacketInfo passes[RENDERING_MAX_NUM_RENDER_PASSES];
        uint8_t passCount{};

        ShaderSetupPacketInfo shaderSetups[RENDERING_MAX_NUM_SHADER_SETUPS];
        uint16_t shaderSetupCount{};

        ShaderPacketInfo shaders[RENDERING_MAX_NUM_SHADERS];
        uint16_t shaderCount{};

        MaterialPacketInfo materials[RENDERING_MAX_NUM_MATERIALS];
        uint16_t materialCount{};

        DrawPacketInfo drawPackets[RENDERING_MAX_NUM_DRAWPACKETS];
        uint32_t drawPacketCount{};

        Ref<TexturePack> texpacks[RENDERING_MAX_NUM_TEXPACKS];
        uint32_t texpackCount{};

        UniformBufferBindInfo uniforms[RENDERING_MAX_NUM_UNIFORMS];
        uint32_t uniformCount{};

        DrawCallInfo drawCalls[RENDERING_MAX_NUM_DRAW_CALLS]{};
        uint32_t drawCallCount{};
    };

    struct RenderPacketDrawProcess
    {
        RenderPassPacketInfo* currentPass{};
        ShaderSetupPacketInfo* currentShaderSetup{};
        ShaderPacketInfo* currentShader{};
        MaterialPacketInfo* currentMaterial{};
        DrawPacketInfo* currentDraw{};

        RenderPacket packet{};
    };
}

#endif //HPACKETS_H
