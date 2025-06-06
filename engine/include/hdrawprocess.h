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

        uint32_t uniformStart{};
        uint32_t uniformCount{};

        uint32_t drawCallStart{};
        uint32_t drawCallCount{};
    };

    struct ShaderPacketInfo
    {
        ShaderBindingInfo bindingInfo{};
        uint32_t drawPacketStart{};
        uint32_t drawPacketCount{};
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
        DrawPacketInfo* currentDraw{};

        RenderPacket packet{};
    };

    void StartRenderPassPacket(const Ref<Renderer>& rn, RenderPass pass);
    void EndRenderPassPacket(const Ref<Renderer>& rn);

    void StartShaderSetupPacket(const Ref<Renderer>& rn, ShaderSetup shaderSetup);
    void EndShaderSetupPacket(const Ref<Renderer>& rn);

    void StartShaderPacket(const Ref<Renderer>& rn, const ShaderBindingInfo& shaderBindingInfo);
    void EndShaderPacket(const Ref<Renderer>& rn);

    void StartDrawPacket(const Ref<Renderer>& rn);
    void EndDrawPacket(const Ref<Renderer>& rn);

    void ShaderSetupAdd_UniformBinding(const Ref<Renderer>& rn, const UniformBufferBindInfo& uniformBinding);
    void ShaderSetupAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack);

    void PacketAdd_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh);
    void PacketAdd_DrawCall(const Ref<Renderer>& rn, const DrawCallInfo& drawCall);
    void PacketAdd_UniformBinding(const Ref<Renderer>& rn, const UniformBufferBindInfo& uniformBinding);
    void PacketAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack);
}

#endif //HPACKETS_H
