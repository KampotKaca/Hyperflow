#include "hdrawprocess.h"
#include "hgenericexception.h"
#include "hrenderer.h"
#include "hinternal.h"

namespace hf
{
    namespace inter::rendering
    {
        void StartRenderPacket_i(const Ref<Renderer>& rn)
        {
            rn->isDrawing = true;
        }

        void EndRenderPacket_i(const Ref<Renderer>& rn)
        {
            rn->packetQueue[rn->packetCount] = rn->currentDraw.packet;
            rn->packetCount++;
            rn->currentDraw = {};
            rn->isDrawing = false;
        }
    }

    void StartRenderPassPacket(const Ref<Renderer>& rn, RenderPass pass)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start RenderPass, without ending the previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.passes[packet.passCount] =
        {
            .pass = pass,
            .shaderSetupStart = packet.shaderSetupCount,
            .shaderSetupCount = 0
        };

        currentDraw.currentPass = &packet.passes[packet.passCount];
        packet.passCount++;
    }

    void EndRenderPassPacket(const Ref<Renderer>& rn)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end RenderPass, without starting it!");
#endif

        currentDraw.currentPass = nullptr;
    }

    void StartShaderSetupPacket(const Ref<Renderer>& rn, ShaderSetup shaderSetup)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "RenderPass must be set!");

        if (currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start ShaderSetup without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.shaderSetups[packet.shaderSetupCount] =
        {
            .shaderSetup = shaderSetup,
            .shaderStart = packet.shaderCount,
            .shaderCount = 0
        };

        currentDraw.currentShaderSetup = &packet.shaderSetups[packet.shaderSetupCount];
        packet.shaderSetupCount++;
        currentDraw.currentPass->shaderSetupCount++;
    }

    void EndShaderSetupPacket(const Ref<Renderer>& rn)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "RenderPass must be set!");

        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end ShaderSetup without starting it!");
#endif

        currentDraw.currentShaderSetup = nullptr;
    }

    void StartShaderPacket(const Ref<Renderer>& rn, const Ref<Shader>& shader)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

        if (currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Shader without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.shaders[packet.shaderCount] =
        {
            .shader = shader,
            .drawPacketStart = packet.drawPacketCount,
            .drawPacketCount = 0
        };

        currentDraw.currentShader = &packet.shaders[packet.shaderCount];
        packet.shaderCount++;
        currentDraw.currentShaderSetup->shaderCount++;
    }

    void EndShaderPacket(const Ref<Renderer>& rn)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end Shader without starting it!");
#endif

        currentDraw.currentShader = nullptr;
    }

    void StartDrawPacket(const Ref<Renderer>& rn)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to end draw before starting new one!");
#endif

        auto& packet = currentDraw.packet;
        packet.drawPackets[packet.drawPacketCount] =
        {
            .texpackStart = packet.texpackCount,
            .texpackCount = 0,
            .uniformStart = packet.uniformCount,
            .uniformCount = 0,
            .drawCallStart = packet.drawCallCount,
            .drawCallCount = 0
        };

        currentDraw.currentDraw = &packet.drawPackets[packet.drawPacketCount];
        packet.drawPacketCount++;
        currentDraw.currentShader->drawPacketCount++;
    }

    void EndDrawPacket(const Ref<Renderer>& rn)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to start draw before ending it!");
#endif

        currentDraw.currentDraw = nullptr;
    }

    void PacketAdd_DrawCall(const Ref<Renderer>& rn, const DrawCallInfo& drawCall)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.drawCalls[packet.drawCallCount] = drawCall;
        packet.drawCallCount++;
        currentDraw.currentDraw->drawCallCount++;
    }

    void PacketAdd_UniformBinding(const Ref<Renderer>& rn, const UniformBufferBindInfo& uniformBinding)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.uniforms[packet.uniformCount] = uniformBinding;
        packet.uniformCount++;
        currentDraw.currentDraw->uniformCount++;
    }

    void PacketAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack)
    {
        auto& currentDraw = rn->currentDraw;
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.texpacks[packet.texpackCount] = texPack;
        packet.texpackCount++;
        currentDraw.currentDraw->texpackCount++;
    }
}
