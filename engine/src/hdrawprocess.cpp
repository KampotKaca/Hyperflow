#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hgenericexception.h"
#include "hrenderer.h"
#include "hinternal.h"
#include "hshader.h"
#include "htexturepack.h"
#include "hmaterial.h"

namespace hf
{
    namespace draw
    {
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

        void UploadUniformPacket(const Ref<Renderer>& rn, const UniformBufferUpload& info)
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload uniform util drawing process starts");
#endif

            auto& packet = rn->currentDraw.packet;
            memcpy(&packet.uniformUploads[packet.uniformUploadSize], info.data, info.sizeInBytes);
            packet.uniformUploadPackets[packet.uniformUploadPacketCount] =
            {
                .buffer = info.buffer,
                .offsetInBytes = info.offsetInBytes,
                .uniformStart = packet.uniformUploadSize,
                .uniformSize = info.sizeInBytes
            };

            packet.uniformUploadSize += info.sizeInBytes;
            packet.uniformUploadPacketCount++;
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
                .shaderCount = 0,
                .uniformStart = packet.uniformCount,
                .uniformCount = 0,
                .texpackStart = packet.texpackCount,
                .texpackCount = 0
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

        void StartShaderPacket(const Ref<Renderer>& rn, const ShaderBindingInfo& shaderBindingInfo)
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
                .bindingInfo = shaderBindingInfo,
                .materialPacketStart = packet.materialCount,
                .materialPacketCount = 0
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

        void StartMaterialPacket(const Ref<Renderer>& rn, const Ref<Material>& material)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentShader)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

            if (currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Material without ending previous one!");
#endif

            auto& packet = currentDraw.packet;
            packet.materials[packet.materialCount] =
            {
                .material = material,
                .drawPacketStart = packet.drawPacketCount,
                .drawPacketCount = 0
            };

            currentDraw.currentMaterial = &packet.materials[packet.materialCount];
            packet.materialCount++;
            currentDraw.currentShader->materialPacketCount++;
        }

        void EndMaterialPacket(const Ref<Renderer>& rn)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentShader)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

            if (!currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end material without starting it!");
#endif
            currentDraw.currentMaterial = nullptr;
        }

        void StartDrawPacket(const Ref<Renderer>& rn)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");

            if (currentDraw.currentDraw)
                throw GENERIC_EXCEPT("[Hyperflow]", "Need to end draw before starting new one!");
#endif

            auto& packet = currentDraw.packet;
            packet.drawPackets[packet.drawPacketCount] =
            {
                .texpackStart = packet.texpackCount,
                .texpackCount = 0,
                .drawCallStart = packet.drawCallCount,
                .drawCallCount = 0,
            };

            currentDraw.currentDraw = &packet.drawPackets[packet.drawPacketCount];
            packet.drawPacketCount++;
            currentDraw.currentMaterial->drawPacketCount++;
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

        void ShaderSetupAdd_UniformBinding(const Ref<Renderer>& rn, const UniformBufferBindInfo& uniformBinding)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentShaderSetup)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader Setup must be set!");
#endif

            auto& packet = currentDraw.packet;
            packet.uniforms[packet.uniformCount] = uniformBinding;
            packet.uniformCount++;
            currentDraw.currentShaderSetup->uniformCount++;
        }

        void ShaderSetupAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentShaderSetup)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader Setup must be set!");
#endif

            auto& packet = currentDraw.packet;
            packet.texpacks[packet.texpackCount] = texPack;
            packet.texpackCount++;
            currentDraw.currentShaderSetup->texpackCount++;
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

        void PacketSet_PushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize)
        {
            auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentDraw)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

            currentDraw.currentDraw->pushConstantBuffer[0] = 1;
            memcpy(currentDraw.currentDraw->pushConstantBuffer + 1, data, dataSize);
        }
    }

    namespace inter::rendering
    {
        void StartRenderPacket_i(const Ref<Renderer>& rn)
        {
            rn->isDrawing = true;
        }

        void EndRenderPacket_i(const Ref<Renderer>& rn)
        {
            auto& tInfo = rn->threadInfo;
#if DEBUG
            if (!rn->isDrawing) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end RenderPacket, without starting it!");
#endif

            {
                std::lock_guard lock(tInfo.threadLock);
                tInfo.drawPacket = rn->currentDraw.packet;
                tInfo.packetIsReady = true;
                tInfo.renderCondition.notify_one();
            }
            rn->currentDraw = {};
            rn->isDrawing = false;
        }

        void RendererUpdate_i(const Ref<Renderer>& rn)
        {
            HF.renderingApi.api.WaitForPreviousFrame(rn->handle);
            CleanMarkedResources_i();

            auto& tInfo = rn->threadInfo;
            RenderPacket packet;
            {
                std::unique_lock lock(tInfo.threadLock);
                tInfo.renderCondition.wait(lock, [&]{ return tInfo.packetIsReady || !tInfo.isRunning; });
                if (!tInfo.isRunning) return;
                packet = tInfo.drawPacket;
                tInfo.packetIsReady = false;
            }

            if(HF.renderingApi.api.GetReadyForRendering(rn->handle))
            {
                auto& cInfo = rn->eventInfo;
                if (cInfo.onPreRenderCallback) cInfo.onPreRenderCallback(rn);
                HF.renderingApi.api.StartFrame(rn->handle);
                RendererDraw_i(rn, packet);
                HF.renderingApi.api.EndFrame(rn->handle);
            }
        }

        void RendererDraw_i(const Ref<Renderer>& rn, const RenderPacket& packet)
        {
            void* vBufferCache[MAX_NUM_BUFFER_CACHE];
            auto handle = rn->handle;

            {
                const UniformBufferUploadInfo uploadInfo
                {
                    .pUploadPackets = packet.uniformUploadPackets,
                    .uploadPacketCount = packet.uniformUploadPacketCount,
                    .pUniformDataBuffer = packet.uniformUploads
                };
                HF.renderingApi.api.UploadUniformBuffer(handle, uploadInfo);
            }

            for (uint8_t passIndex = 0; passIndex < packet.passCount; passIndex++)
            {
                const auto& pass = packet.passes[passIndex];
                HF.renderingApi.api.BeginRenderPass(handle, pass.pass);

                uint16_t shaderSetupEnd = pass.shaderSetupStart + pass.shaderSetupCount;
                for (uint16_t shaderSetupIndex = pass.shaderSetupStart; shaderSetupIndex < shaderSetupEnd; shaderSetupIndex++)
                {
                    const auto& shaderSetup = packet.shaderSetups[shaderSetupIndex];
                    HF.renderingApi.api.BindShaderSetup(handle, shaderSetup.shaderSetup);

                    {
                        uint32_t uniformEnd = shaderSetup.uniformStart + shaderSetup.uniformCount;
                        for (uint32_t uniformIndex = shaderSetup.uniformStart; uniformIndex < uniformEnd; uniformIndex++)
                        {
                            const auto& uniform = packet.uniforms[uniformIndex];
                            HF.renderingApi.api.BindUniformBuffer(handle, uniform);
                        }
                    }

                    {
                        uint32_t texturePackEnd = shaderSetup.texpackStart + shaderSetup.texpackCount;
                        for (uint32_t texturePackIndex = shaderSetup.texpackStart; texturePackIndex < texturePackEnd; texturePackIndex++)
                        {
                            auto* texPack = packet.texpacks[texturePackIndex]->handle;
                            if (texPack) HF.renderingApi.api.BindTexturePack(handle, texPack);
                        }
                    }

                    uint16_t shaderEnd = shaderSetup.shaderStart + shaderSetup.shaderCount;
                    for (uint16_t shaderIndex = shaderSetup.shaderStart; shaderIndex < shaderEnd; shaderIndex++)
                    {
                        const auto& shader = packet.shaders[shaderIndex];
                        auto* shaderHandle = shader.bindingInfo.shader->handle;

                        if (shaderHandle)
                        {
                            ShaderBindingInfo shaderInfo
                            {
                                .shader = shaderHandle,
                                .attrib = shader.bindingInfo.attrib,
                                .bindingPoint = shader.bindingInfo.bindingPoint
                            };
                            HF.renderingApi.api.BindShader(handle, shaderInfo);

                            uint16_t materialEnd = shader.materialPacketStart + shader.materialPacketCount;
                            for (uint16_t materialIndex = shader.materialPacketStart; materialIndex < materialEnd; materialIndex++)
                            {
                                auto& material = packet.materials[materialIndex];

                                uint32_t drawPacketEnd = material.drawPacketStart + material.drawPacketCount;
                                for (uint32_t drawPacketIndex = material.drawPacketStart; drawPacketIndex < drawPacketEnd; drawPacketIndex++)
                                {
                                    auto& drawPacket = packet.drawPackets[drawPacketIndex];

                                    if (drawPacket.pushConstantBuffer[0] == 1)
                                    {
                                        PushConstantUploadInfo uploadInfo
                                        {
                                            .shaderSetup = shaderSetup.shaderSetup,
                                            .data = &drawPacket.pushConstantBuffer[1]
                                        };

                                        HF.renderingApi.api.UploadPushConstants(handle, uploadInfo);
                                    }

                                    uint32_t texpackEnd = drawPacket.texpackStart + drawPacket.texpackCount;
                                    for (uint32_t texpackIndex = drawPacket.texpackStart; texpackIndex < texpackEnd; texpackIndex++)
                                    {
                                        auto* texPack = packet.texpacks[texpackIndex]->handle;
                                        if (texPack) HF.renderingApi.api.BindTexturePack(handle, texPack);
                                    }

                                    uint32_t drawCallEnd = drawPacket.drawCallStart + drawPacket.drawCallCount;
                                    for (uint32_t drawCallIndex = drawPacket.drawCallStart; drawCallIndex < drawCallEnd; drawCallIndex++)
                                    {
                                        const auto& drawCall = packet.drawCalls[drawCallIndex];

#if DEBUG
                                        if (drawCall.bufferCount > MAX_NUM_BUFFER_CACHE)
                                            throw GENERIC_EXCEPT("[Hyperflow]", "Trying to draw too many buffers at once, max is %i", MAX_NUM_BUFFER_CACHE);
#endif

                                        for (uint32_t i = 0; i < drawCall.bufferCount; i++)
                                            vBufferCache[i] = drawCall.pVertBuffers[i]->handle;

                                        DrawCallInfo drawInfo
                                        {
                                            .pVertBuffers = vBufferCache,
                                            .bufferCount = drawCall.bufferCount,
                                            .indexBuffer = drawCall.indexBuffer ? drawCall.indexBuffer->handle : nullptr,
                                            .instanceCount = drawCall.instanceCount
                                        };
                                        HF.renderingApi.api.Draw(handle, drawInfo);
                                    }
                                }
                            }
                        }
                    }
                }

                HF.renderingApi.api.EndRenderPass(handle);
            }
        }
    }
}
