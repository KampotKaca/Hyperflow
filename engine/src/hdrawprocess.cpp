#define HF_ENGINE_INTERNALS
#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "hinternal.h"
#include "hshader.h"
#include "htexturepack.h"
#include "hmaterial.h"

namespace hf
{
    void Renderer::Upload_Uniform(const UniformBufferUpload& info)
    {
#if DEBUG
        if (!isDrawing)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload uniform util drawing process starts");
#endif

        auto& packet = currentDraw.packet;
        packet.uniformUploadPackets.push_back(
        {
            .buffer = info.buffer,
            .offsetInBytes = info.offsetInBytes,
            .uniformRange = (AssetRange<uint32_t>) { .start = (uint32_t)packet.uniformUploads.size(), .size = info.sizeInBytes }
        });

        packet.uniformUploads.push_back((uint8_t*)info.data, info.sizeInBytes);
    }

    void Renderer::UploadStart_TexturePack(const Ref<TexturePack>& texPack)
    {
#if DEBUG
        if (!isDrawing)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload texture pack util drawing process starts");
        if (currentDraw.currentTexturePackBinding)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start texture pack upload util you end previous one");
#endif

        auto& packet = currentDraw.packet;
        packet.textureGroupRebindings.push_back(TexturePackRebindingGroupPacketInfo
        {
            .texturePack = texPack,
            .bindingPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.textureRebindings.size(), .size = 0 }
        });
        currentDraw.currentTexturePackBinding = packet.textureGroupRebindings.atP(packet.textureGroupRebindings.size() - 1);
    }

    void Renderer::UploadEnd_TexturePack()
    {
#if DEBUG
        if (!currentDraw.currentTexturePackBinding)
            throw GENERIC_EXCEPT("[Hyperflow]", "You need to start texture pack first");
#endif
        currentDraw.currentTexturePackBinding = nullptr;
    }

    template void Renderer::UploadAdd_TexturePackBinding(const TexturePackBindingUploadInfo<Texture>& info);
    template void Renderer::UploadAdd_TexturePackBinding(const TexturePackBindingUploadInfo<Cubemap>& info);
    template<typename T>
    void Renderer::UploadAdd_TexturePackBinding(const TexturePackBindingUploadInfo<T>& info)
    {
#if DEBUG
        if (!currentDraw.currentTexturePackBinding)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add binding packet without starting texture pack upload");
#endif
        auto& packet = currentDraw.packet;
        auto& b = currentDraw.currentTexturePackBinding->texturePack->bindings[info.bindingIndex];

        bool wasModified = false;
        auto binding = (TexturePackRebindingPacketInfo)
        {
            .bindingIndex = info.bindingIndex,
            .sampler = info.sampler,
        };

        if (info.sampler.has_value())
        {
            b.sampler = info.sampler.value();
            wasModified = true;
        }

        if (info.texInfo.has_value())
        {
            auto textures = info.texInfo.value();
            for (uint32_t i = 0; i < textures.count; i++)
                b.textures[textures.offset + i] = textures.pTextures[i]->handle;

            binding.textures = (TexturePackRebindingPacketInfo::TextureBinding)
            {
                .offset = textures.offset,
                .type = TexturePackBindingType::Texture2D,
                .range = (AssetRange<uint16_t>){ .start = (uint16_t)packet.textures.size(), .size = (uint16_t)textures.count }
            };

            for (uint32_t i = 0; i < textures.count; i++)
                packet.textures.push_back(textures.pTextures[i]->handle);
            if (textures.count > 0) wasModified = true;
        }

        if (wasModified)
        {
            packet.textureRebindings.push_back(binding);
            currentDraw.currentTexturePackBinding->bindingPacketRange.size++;
        }
        else LOG_WARN("Unnecessary binding upload! nothing changed");
    }

    void Renderer::Start_RenderPass(RenderPass pass)
    {
#if DEBUG
        if (currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start RenderPass, without ending the previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.passes.push_back({
            .pass = pass,
            .shaderSetupRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.shaderSetups.size(), .size = 0 }
        });

        currentDraw.currentPass = packet.passes.atP(packet.passes.size() - 1);
    }

    void Renderer::End_RenderPass()
    {
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end RenderPass, without starting it!");
#endif

        currentDraw.currentPass = nullptr;
    }

    void Renderer::Start_ShaderSetup(ShaderSetup shaderSetup)
    {
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "RenderPass must be set!");

        if (currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start ShaderSetup without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.shaderSetups.push_back({
            .shaderSetup = shaderSetup,
            .shaderPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.shaders.size(), .size = 0 } ,
            .uniformSetRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.uniformSets.size(), .size = 0 } ,
        });

        currentDraw.currentShaderSetup = packet.shaderSetups.atP(packet.shaderSetups.size() - 1);
        currentDraw.currentPass->shaderSetupRange.size++;
    }

    void Renderer::End_ShaderSetup()
    {
#if DEBUG
        if (!currentDraw.currentPass)
            throw GENERIC_EXCEPT("[Hyperflow]", "RenderPass must be set!");

        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end ShaderSetup without starting it!");
#endif

        currentDraw.currentShaderSetup = nullptr;
    }

    void Renderer::Start_Shader(const ShaderBindingInfo& shaderBindingInfo)
    {
#if DEBUG
        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

        if (currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Shader without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.shaders.push_back({
            .bindingInfo = shaderBindingInfo,
            .materialPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.materials.size(), .size = 0 }
        });

        currentDraw.currentShader = packet.shaders.atP(packet.shaders.size() - 1);
        currentDraw.currentShaderSetup->shaderPacketRange.size++;
    }

    void Renderer::End_Shader()
    {
#if DEBUG
        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end Shader without starting it!");
#endif

        currentDraw.currentShader = nullptr;
    }

    void Renderer::Start_Material(const Ref<Material>& material)
    {
#if DEBUG
        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (currentDraw.currentMaterial)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Material without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.materials.push_back({
            .material = material,
            .drawPacketRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.drawPackets.size(), .size = 0 },
            .texpackRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.texpacks.size(), .size = 0 },
        });

        currentDraw.currentMaterial = packet.materials.atP(packet.materials.size() - 1);
        currentDraw.currentShader->materialPacketRange.size++;
    }

    void Renderer::End_Material()
    {
#if DEBUG
        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!currentDraw.currentMaterial)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end material without starting it!");
#endif
        currentDraw.currentMaterial = nullptr;
    }

    void Renderer::Start_Draw()
    {
#if DEBUG
        if (!currentDraw.currentMaterial)
            throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");

        if (currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to end draw before starting new one!");
#endif

        auto& packet = currentDraw.packet;
        packet.drawPackets.push_back({
            .texpackRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.texpacks.size(), .size = 0 },
            .drawCallRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.drawCalls.size(), .size = 0 },
            .pushConstantRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.pushConstantUploads.size(), .size = 0 },
        });

        currentDraw.currentDraw = packet.drawPackets.atP(packet.drawPackets.size() - 1);
        currentDraw.currentMaterial->drawPacketRange.size++;
    }

    void Renderer::End_Draw()
    {
#if DEBUG
        if (!currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to start draw before ending it!");
#endif

        currentDraw.currentDraw = nullptr;
    }

    void Renderer::Start_UniformSet(RenderBindingType bindingType, uint32_t setBindingIndex)
    {
#if DEBUG
        if (!currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader Setup must be set until you set uniform set");
        if (currentDraw.currentUniformSet)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start uniform set without ending previous one!");
#endif

        auto& packet = currentDraw.packet;
        packet.uniformSets.push_back(UniformBufferSetPacketInfo
        {
            .bindingType = bindingType,
            .setBindingIndex = setBindingIndex,
            .bufferRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.uniformBuffers.size(), .size = 0 },
        });
        currentDraw.currentUniformSet = packet.uniformSets.atP(packet.uniformSets.size() - 1);
        currentDraw.currentShaderSetup->uniformSetRange.size++;
    }

    void Renderer::End_UniformSet()
    {
#if DEBUG
        if (!currentDraw.currentUniformSet)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end uniform set without starting it!");
#endif
        currentDraw.currentUniformSet = nullptr;
    }

    void Renderer::UniformSetAdd_Uniform(UniformBuffer buffer)
    {
#if DEBUG
        if (!currentDraw.currentUniformSet)
            throw GENERIC_EXCEPT("[Hyperflow]", "Uniform Set must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.uniformBuffers.push_back(buffer);
        currentDraw.currentUniformSet->bufferRange.size++;
    }

    void Renderer::MaterialAdd_TexturePackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
    {
#if DEBUG
        if (!currentDraw.currentMaterial)
            throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.texpacks.push_back({
            .pack = texPack,
            .setBindingIndex = setBindingIndex,
        });
        currentDraw.currentMaterial->texpackRange.size++;
    }

    void Renderer::DrawAdd_DrawCall(const Ref<Mesh>& mesh)
    {
        for (auto& submesh : mesh->subMeshes)
        {
            DrawCallInfo drawInfo
            {
                .pVertBuffers = &submesh.vertBuffer,
                .bufferCount = 1,
                .indexBuffer = submesh.indexBuffer,
                .instanceCount = 1
            };
            DrawAdd_DrawCall(drawInfo);
        }
    }

    void Renderer::DrawAdd_DrawCall(const DrawCallInfo& drawCall)
    {
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.drawCalls.push_back(drawCall);
        currentDraw.currentDraw->drawCallRange.size++;
    }

    void Renderer::DrawAdd_TexturePackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
    {
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

        auto& packet = currentDraw.packet;
        packet.texpacks.push_back({
            .pack = texPack,
            .setBindingIndex = setBindingIndex
        });
        currentDraw.currentDraw->texpackRange.size++;
    }

    void Renderer::DrawSet_PushConstant(const void* data, uint32_t dataSize)
    {
#if DEBUG
        if (!currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");

        if (currentDraw.currentDraw->pushConstantRange.size > 0)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot set push constant twice for a single draw packet!");
#endif

        auto& packet = currentDraw.packet;
        currentDraw.currentDraw->pushConstantRange.size = dataSize;
        packet.pushConstantUploads.push_back((uint8_t*)data, dataSize);
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
                HF.renderingApi.api.StartFrame(rn->handle);
                RendererDraw_i(rn, packet);
                HF.renderingApi.api.EndFrame(rn->handle);
            }
        }

        void RendererDraw_i(const Ref<Renderer>& rn, RenderPacket& packet)
        {
            void* vBufferCache[MAX_NUM_BUFFER_CACHE];
            auto handle = rn->handle;

            {
                const UniformBufferUploadInfo uploadInfo
                {
                    .pUploadPackets = packet.uniformUploadPackets.begin(),
                    .uploadPacketCount = (uint16_t)packet.uniformUploadPackets.size(),
                    .pUniformDataBuffer = packet.uniformUploads.begin()
                };
                HF.renderingApi.api.UploadUniformBuffer(handle, uploadInfo);
            }

            {
                static TexturePackBindingUploadInfo uploads[128];
                for (uint32_t i = 0; i < packet.textureGroupRebindings.size(); i++)
                {
                    const auto& group = packet.textureGroupRebindings.atC(i);
                    TexturePackBindingUploadGroupInfo uploadInfo
                    {
                        .bindings = uploads,
                        .bindingCount = group.bindingPacketRange.size
                    };

                    uint32_t end = group.bindingPacketRange.end();
                    uint32_t current = 0;
                    for (uint32_t j = group.bindingPacketRange.start; j < end; j++)
                    {
                        auto& p = packet.textureRebindings.atC(j);
                        TexturePackBindingUploadInfo binding
                        {
                            .bindingIndex = p.bindingIndex,
                            .sampler = p.sampler,
                        };

                        if (p.textures.has_value())
                        {
                            auto& textures = p.textures.value();
                            binding.texInfo = (TexturePackTextureUploadInfo)
                            {
                                .pTextures = packet.textures.atP(textures.range.start),
                                .offset = textures.offset,
                                .count = textures.range.size
                            };
                        }

                        uploads[current] = binding;
                        current++;
                    }

                    HF.renderingApi.api.UploadTexturePackBinding(group.texturePack->handle, uploadInfo);
                }
            }

            for (uint8_t passIndex = 0; passIndex < (uint8_t)packet.passes.size(); passIndex++)
            {
                auto& pass = packet.passes.atC(passIndex);
                HF.renderingApi.api.BeginRenderPass(handle, pass.pass);

                uint16_t shaderSetupEnd = pass.shaderSetupRange.end();
                for (uint16_t shaderSetupIndex = pass.shaderSetupRange.start; shaderSetupIndex < shaderSetupEnd; shaderSetupIndex++)
                {
                    Ref<TexturePack> boundTexturePacks[MAX_NUM_BOUND_TEXTUREPACKS]{};

                    const auto& shaderSetup = packet.shaderSetups.atC(shaderSetupIndex);
                    HF.renderingApi.api.BindShaderSetup(handle, shaderSetup.shaderSetup);

                    {
                        uint32_t uniformEnd = shaderSetup.uniformSetRange.end();
                        for (uint32_t uniformIndex = shaderSetup.uniformSetRange.start; uniformIndex < uniformEnd; uniformIndex++)
                        {
                            const auto& uniformSet = packet.uniformSets.atC(uniformIndex);
                            UniformBufferBindInfo info
                            {
                                .bindingType = uniformSet.bindingType,
                                .setBindingIndex = uniformSet.setBindingIndex,
                                .pUniforms = packet.uniformBuffers.atP(uniformSet.bufferRange.start),
                                .uniformCount = uniformSet.bufferRange.size
                            };
                            HF.renderingApi.api.BindUniformBuffer(handle, info);
                        }
                    }

                    uint16_t shaderEnd = shaderSetup.shaderPacketRange.end();
                    for (uint16_t shaderIndex = shaderSetup.shaderPacketRange.start; shaderIndex < shaderEnd; shaderIndex++)
                    {
                        const auto& shader = packet.shaders.atC(shaderIndex);
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

                            uint16_t materialEnd = shader.materialPacketRange.end();
                            for (uint16_t materialIndex = shader.materialPacketRange.start; materialIndex < materialEnd; materialIndex++)
                            {
                                auto& material = packet.materials.atC(materialIndex);

                                uint32_t drawPacketEnd = material.drawPacketRange.end();
                                for (uint32_t drawPacketIndex = material.drawPacketRange.start; drawPacketIndex < drawPacketEnd; drawPacketIndex++)
                                {
                                    auto& drawPacket = packet.drawPackets.atC(drawPacketIndex);

                                    if (drawPacket.pushConstantRange.size > 0)
                                    {
                                        PushConstantUploadInfo uploadInfo
                                        {
                                            .shaderSetup = shaderSetup.shaderSetup,
                                            .data = &packet.pushConstantUploads.atC(drawPacket.pushConstantRange.start),
                                        };

                                        HF.renderingApi.api.UploadPushConstants(handle, uploadInfo);
                                    }

                                    {// Binding Texture packs
                                        Ref<TexturePack> currentTexturePacks[MAX_NUM_BOUND_TEXTUREPACKS]{};
                                        {
                                            const uint32_t texpackEnd = material.texpackRange.end();
                                            for (uint32_t texpackIndex = material.texpackRange.start; texpackIndex < texpackEnd; texpackIndex++)
                                            {
                                                const auto texPack = packet.texpacks.atC(texpackIndex);
                                                currentTexturePacks[texPack.setBindingIndex] = texPack.pack;
                                            }
                                        }

                                        {
                                            const uint32_t texpackEnd = drawPacket.texpackRange.end();
                                            for (uint32_t texpackIndex = drawPacket.texpackRange.start; texpackIndex < texpackEnd; texpackIndex++)
                                            {
                                                const auto texPack = packet.texpacks.atC(texpackIndex);
                                                currentTexturePacks[texPack.setBindingIndex] = texPack.pack;
                                            }
                                        }

                                        for (uint32_t i = 0; i < MAX_NUM_BOUND_TEXTUREPACKS; i++)
                                        {
                                            auto cPack = currentTexturePacks[i];
                                            if (cPack != boundTexturePacks[i] && cPack && cPack->handle)
                                            {
                                                TexturePackBindingInfo info
                                                {
                                                    .texturePack = cPack->handle,
                                                    .setBindingIndex = i
                                                };

                                                HF.renderingApi.api.BindTexturePack(handle, info);
                                                boundTexturePacks[i] = cPack;
                                            }
                                        }
                                    }

                                    uint32_t drawCallEnd = drawPacket.drawCallRange.end();
                                    for (uint32_t drawCallIndex = drawPacket.drawCallRange.start; drawCallIndex < drawCallEnd; drawCallIndex++)
                                    {
                                        const auto& drawCall = packet.drawCalls.atC(drawCallIndex);

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
