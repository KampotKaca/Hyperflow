#include "hdrawprocess.h"
#include "hyperflow.h"
#include "hrenderer.h"
#include "hinternal.h"
#include "hshader.h"
#include "htexturepack.h"
#include "hmaterial.h"
#include "hmesh.h"
#include "hrendertexture.h"
#include "htexture.h"

namespace hf
{
#if DEBUG

    void Set_DrawCallback(const Ref<Renderer>& rn, void (*callback)(const Ref<Renderer>&, void*))
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot draw anything without render texture");
#endif

            rn->currentDraw.currentRenderTexture->drawCallback = callback;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to set draw callback!");
            throw;
        }
    }

#endif

    void Upload_Buffer(const Ref<Renderer>& rn, const BufferUploadInfo& info)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload uniform util drawing process starts");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.bufferUploadPackets.push_back(
            {
                .buffer = info.buffer,
                .offsetInBytes = info.offsetInBytes,
                .bufferRange = (AssetRange<uint32_t>) { .start = (uint32_t)packet.bufferUploads.size(), .size = info.sizeInBytes }
            });

            packet.bufferUploads.push_back((uint8_t*)info.data, info.sizeInBytes);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to upload more more buffers!");
            throw;
        }
    }

    void Upload_Material(const Ref<Renderer>& rn, const Ref<Material>& material)
    {
        try
        {
            const BufferUploadInfo info
            {
                .buffer = inter::HF.graphicsResources.materialDataStorageBuffer,
                .offsetInBytes = material->bufferIndex * RENDERING_MAX_MATERIAL_MEMORY_BADGET,
                .sizeInBytes = material->sizeInBytes,
                .data = material->bufferMemory
            };

            Upload_Buffer(rn, info);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to upload material!");
            throw;
        }
    }

    void UploadStart_TexturePack(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload texture pack util drawing process starts");
            if (rn->currentDraw.currentTexturePackBinding)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start texture pack upload util you end previous one");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.textureGroupRebindings.push_back(TexturePackRebindingGroupPacketInfo
            {
                .texturePack = texPack,
                .bindingPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.textureRebindings.size(), .size = 0 }
            });
            rn->currentDraw.currentTexturePackBinding = packet.textureGroupRebindings.atP(packet.textureGroupRebindings.size() - 1);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to upload texture pack!");
            throw;
        }
    }

    void UploadEnd_TexturePack(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentTexturePackBinding)
            throw GENERIC_EXCEPT("[Hyperflow]", "You need to start texture pack first");
#endif
        rn->currentDraw.currentTexturePackBinding = nullptr;
    }

    template<typename T>
    static void UploadTexture(const Ref<Renderer>& rn, StaticVector<TexturePack::Binding<T>, MAX_TEXTURES_IN_TEXTURE_PACK>& bindings,
                              const TexturePackBindingUploadInfo<T>& info, TexturePackBindingType type)
    {
        auto& packet = rn->currentDraw.packet;
        auto& b = bindings.at(info.bindingIndex);

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
            {
                auto texInfo = textures.pTextures[i];
                b.textures.at(textures.offset + i) =
                {
                    .texture = texInfo.texture,
                    .index = texInfo.index
                };
            }

            binding.textures = (TexturePackRebindingPacketInfo::TextureBinding)
            {
                .offset = textures.offset,
                .type = type,
                .range = (AssetRange<uint16_t>){ .start = (uint16_t)packet.textures.size(), .size = (uint16_t)textures.count }
            };

            for (uint32_t i = 0; i < textures.count; i++)
            {
                auto texture = textures.pTextures[i];
                packet.textures.push_back({
                    .texture = texture.texture->handle,
                    .index = texture.index
                });
            }
            if (textures.count > 0) wasModified = true;
        }

        if (wasModified)
        {
            packet.textureRebindings.push_back(binding);
            rn->currentDraw.currentTexturePackBinding->bindingPacketRange.size++;
        }
        else LOG_WARN("Unnecessary binding upload! nothing changed");
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Texture>& info)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentTexturePackBinding)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add binding packet without starting texture pack upload");
#endif

            UploadTexture(rn, rn->currentDraw.currentTexturePackBinding->texturePack->textureBindings,
                info, TexturePackBindingType::Texture2D);

        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add texture pack texture2d binding!");
            throw;
        }
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Cubemap>& info)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentTexturePackBinding)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add binding packet without starting texture pack upload");
#endif

            UploadTexture(rn, rn->currentDraw.currentTexturePackBinding->texturePack->cubemapBindings,
                info, TexturePackBindingType::Cubemap);

        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add texture pack cubemap binding!");
            throw;
        }
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<RenderTexture>& info)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentTexturePackBinding)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add binding packet without starting texture pack upload");
#endif

            UploadTexture(rn, rn->currentDraw.currentTexturePackBinding->texturePack->renderTextureBindings,
                info, TexturePackBindingType::RenderTexture);

        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add texture pack render texture binding!");
            throw;
        }
    }

    void Add_RenderTextureDependency(const Ref<Renderer>& rn, const RenderAttachmentDependencyInfo& info)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "Can't have any dependencies without any render texture!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.dependencies.push_back(info);
            rn->currentDraw.currentRenderTexture->dependencyRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add render dependency!");
            throw;
        }
    }

    void Start_RenderTexture(const Ref<Renderer>& rn, const Ref<RenderTexture>& rt)
    {
        try
        {
#if DEBUG
            if (rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "You are already drawing on render texture!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.renderTextures.push_back({
                .texture = rt,
                .shaderSetupRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.shaderSetups.size(), .size = 0 },
                .dependencyRange = (AssetRange<uint8_t>){ .start = (uint8_t)packet.dependencies.size(), .size = 0 }
            });

            rn->currentDraw.currentRenderTexture = packet.renderTextures.atP(packet.renderTextures.size() - 1);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start render texture!");
            throw;
        }
    }

    void End_RenderTexture(const Ref<Renderer>& rn)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "Can't end render texture drawing without starting it!");
#endif

            rn->currentDraw.currentRenderTexture = nullptr;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to end render texture!");
            throw;
        }
    }

    void Start_ShaderSetup(const Ref<Renderer>& rn, ShaderSetup shaderSetup)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot draw anything without starting render texture!");

            if (rn->currentDraw.currentShaderSetup)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start ShaderSetup without ending previous one!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.shaderSetups.push_back({
                .shaderSetup = shaderSetup,
                .shaderPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.shaders.size(), .size = 0 },
                .bufferSetRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.bufferSets.size(), .size = 0 },
            });

            rn->currentDraw.currentShaderSetup = packet.shaderSetups.atP(packet.shaderSetups.size() - 1);
            rn->currentDraw.currentRenderTexture->shaderSetupRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start shader setup!");
            throw;
        }
    }

    void End_ShaderSetup(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end ShaderSetup without starting it!");
#endif

        rn->currentDraw.currentShaderSetup = nullptr;
    }

    void Start_Shader(const Ref<Renderer>& rn, const ShaderBindingInfo& shaderBindingInfo)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentShaderSetup)
                throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

            if (rn->currentDraw.currentShader)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Shader without ending previous one!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.shaders.push_back({
                .bindingInfo = shaderBindingInfo,
                .materialPacketRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.materials.size(), .size = 0 }
            });

            rn->currentDraw.currentShader = packet.shaders.atP(packet.shaders.size() - 1);
            rn->currentDraw.currentShaderSetup->shaderPacketRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start shader!");
            throw;
        }
    }

    void End_Shader(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShaderSetup)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderSetup must be set!");

        if (!rn->currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end Shader without starting it!");
#endif

        rn->currentDraw.currentShader = nullptr;
    }

    void Start_Material(const Ref<Renderer>& rn, const Ref<Material>& material)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentShader)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

            if (rn->currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Material without ending previous one!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.materials.push_back({
                .material = material,
                .drawPacketRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.drawPackets.size(), .size = 0 },
                .texpackRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.texpacks.size(), .size = 0 },
            });

            rn->currentDraw.currentMaterial = packet.materials.atP(packet.materials.size() - 1);
            rn->currentDraw.currentShader->materialPacketRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start material!");
            throw;
        }
    }

    void End_Material(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!rn->currentDraw.currentMaterial)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end material without starting it!");
#endif
        rn->currentDraw.currentMaterial = nullptr;
    }

    void Start_Draw(const Ref<Renderer>& rn)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");

            if (rn->currentDraw.currentDraw)
                throw GENERIC_EXCEPT("[Hyperflow]", "Need to end draw before starting new one!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.drawPackets.push_back({
                .texpackRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.texpacks.size(), .size = 0 },
                .drawCallRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.drawCalls.size(), .size = 0 },
                .pushConstantRange = (AssetRange<uint32_t>){ .start = (uint32_t)packet.pushConstantUploads.size(), .size = 0 },
            });

            rn->currentDraw.currentDraw = packet.drawPackets.atP(packet.drawPackets.size() - 1);
            rn->currentDraw.currentMaterial->drawPacketRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("Unable to start draw! Draw packet buffer usage: %f",
                rn->currentDraw.packet.drawPackets.size() / (float)RENDERING_MAX_NUM_DRAWPACKETS);
            throw;
        }
    }

    void End_Draw(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!rn->currentDraw.currentDraw)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to start draw before ending it!");
#endif

        rn->currentDraw.currentDraw = nullptr;
    }

    void Start_BufferSet(const Ref<Renderer>& rn, RenderBindingType bindingType, uint32_t setBindingIndex)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentShaderSetup)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader Setup must be set until you set uniform set");
            if (rn->currentDraw.currentUniformSet)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start uniform set without ending previous one!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.bufferSets.push_back(BufferSetPacketInfo
            {
                .bindingType = bindingType,
                .setBindingIndex = setBindingIndex,
                .bufferRange = (AssetRange<uint16_t>){ .start = (uint16_t)packet.buffers.size(), .size = 0 },
            });
            rn->currentDraw.currentUniformSet = packet.bufferSets.atP(packet.bufferSets.size() - 1);
            rn->currentDraw.currentShaderSetup->bufferSetRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start bufferSet!");
            throw;
        }
    }

    void End_BufferSet(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentUniformSet)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end uniform set without starting it!");
#endif
        rn->currentDraw.currentUniformSet = nullptr;
    }

    void BufferSetAdd_Buffer(const Ref<Renderer>& rn, Buffer buffer)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentUniformSet)
                throw GENERIC_EXCEPT("[Hyperflow]", "Uniform Set must be set!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.buffers.push_back(buffer);
            rn->currentDraw.currentUniformSet->bufferRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add buffer to buffer set!");
            throw;
        }
    }

    void MaterialAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.texpacks.push_back({
                .pack = texPack,
                .setBindingIndex = setBindingIndex,
            });
            rn->currentDraw.currentMaterial->texpackRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to bind texture pack to material!");
            throw;
        }
    }

    void DrawAdd_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh)
    {
        try
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
#if DEBUG
                strcpy(drawInfo.debugName, mesh->filePath.c_str());
#endif
                DrawAdd_DrawCall(rn, drawInfo);
            }
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to Add Draw Call!");
            throw;
        }
    }

    void DrawAdd_DrawCall(const Ref<Renderer>& rn, const DrawCallInfo& drawCall)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentDraw)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.drawCalls.push_back(drawCall);
            rn->currentDraw.currentDraw->drawCallRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to Add Draw Call!");
            throw;
        }
    }

    void DrawAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentDraw)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

            auto& packet = rn->currentDraw.packet;
            packet.texpacks.push_back({
                .pack = texPack,
                .setBindingIndex = setBindingIndex
            });
            rn->currentDraw.currentDraw->texpackRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to bind texture pack to draw!");
            throw;
        }
    }

    void DrawSet_PushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize)
    {
        try
        {
            auto& currentDraw = rn->currentDraw;
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
        catch (...)
        {
            LOG_ERROR("%s", "Unable to set push constant!");
            throw;
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

        void PreDraw_i(const Ref<Renderer>& rn)
        {
            static std::vector<uint64_t> deletedMaterials{};
            deletedMaterials.clear();
            for (auto& material : std::views::values(HF.graphicsResources.materials))
            {
                if (material.use_count() == 1) deletedMaterials.push_back((uint64_t)material.get());
                else
                {
                    if (material->isMarkedAsModified)
                    {
                        Upload_Material(rn, material);
                        material->isMarkedAsModified = false;
                    }
                }
            }

            for (auto& material : deletedMaterials) HF.graphicsResources.materials.erase(material);
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

            void* renderTextures[RENDERING_MAX_NUM_RENDER_TEXTURES];
            for (uint8_t rtIndex = 0; rtIndex < (uint8_t)packet.renderTextures.size(); rtIndex++)
                renderTextures[rtIndex] = packet.renderTextures.atC(rtIndex).texture->handle;

            uvec2 size = HF.renderingApi.api.GetReadyForRendering(rn->handle, renderTextures, packet.renderTextures.size());
            if (size != uvec2(0, 0))
            {
                for (uint8_t rtIndex = 0; rtIndex < (uint8_t)packet.renderTextures.size(); rtIndex++)
                    packet.renderTextures.atC(rtIndex).texture->createInfo.size = size;

// #if DEBUG
                HF.staticResources.debugRenderTexture->createInfo.size = size;
// #endif

                HF.renderingApi.api.StartFrame(rn->handle);
                RendererDraw_i(rn, packet);
                HF.renderingApi.api.EndFrame(rn->handle);
            }
        }

        static void RendererDrawUploads(const void* handle, RenderPacket& packet);
        static void BindBuffers(const void* handle, RenderPacket& packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);
        static void CollectTexpacks(RenderPacket& packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);

        void RendererDraw_i(const Ref<Renderer>& rn, RenderPacket& packet)
        {
            void* vBufferCache[MAX_NUM_BUFFER_CACHE];
            const auto handle = rn->handle;
            RendererDrawUploads(handle, packet);

            for (uint8_t rtIndex = 0; rtIndex < (uint8_t)packet.renderTextures.size(); rtIndex++)
            {
                auto& rt = packet.renderTextures.atC(rtIndex);
                if (rt.dependencyRange.size > 0)
                {
                    HF.renderingApi.api.ApplyRenderAttachmentDependencies(handle,
                    packet.dependencies.atP(rt.dependencyRange.start), rt.dependencyRange.size);
                }

                HF.renderingApi.api.BeginRendering(handle, rt.texture->handle);

                uint16_t shaderSetupEnd = rt.shaderSetupRange.end();
                for (uint16_t shaderSetupIndex = rt.shaderSetupRange.start; shaderSetupIndex < shaderSetupEnd; shaderSetupIndex++)
                {
                    DescriptorBindingInfo descBindings[MAX_NUM_BOUND_DESCRIPTORS]{};

                    const auto& shaderSetup = packet.shaderSetups.atC(shaderSetupIndex);
                    HF.renderingApi.api.BindShaderSetup(handle, shaderSetup.shaderSetup);

                    BindBuffers(handle, packet, shaderSetup.bufferSetRange, descBindings);

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

                            const uint16_t materialEnd = shader.materialPacketRange.end();
                            for (uint16_t materialIndex = shader.materialPacketRange.start; materialIndex < materialEnd; materialIndex++)
                            {
                                auto& material = packet.materials.atC(materialIndex);

                                if (material.material->sizeInBytes > 0)
                                {
                                    BufferBindInfo info
                                    {
                                        .bindingType = RenderBindingType::Graphics,
                                        .setBindingIndex = 1,
                                        .pBuffers = &HF.graphicsResources.materialDataStorageBuffer,
                                        .bufferCount = 1
                                    };

                                    HF.renderingApi.api.BindBuffer(handle, info);
                                }

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
                                        DescriptorBindingInfo currentDescriptors[MAX_NUM_BOUND_DESCRIPTORS]{};
                                        CollectTexpacks(packet, material.texpackRange, currentDescriptors);
                                        CollectTexpacks(packet, drawPacket.texpackRange, currentDescriptors);

                                        for (uint32_t i = 0; i < MAX_NUM_BOUND_DESCRIPTORS; i++)
                                        {
                                            auto cPack = currentDescriptors[i];
                                            if (cPack.object != descBindings[i].object && cPack.object)
                                            {
                                                switch (cPack.type)
                                                {
                                                    case BUFFER: throw GENERIC_EXCEPT("[Hyperflow]", "Cannot bind buffer here!");
                                                    case TEXPACK:
                                                        {
                                                            TexturePackBindingInfo info
                                                            {
                                                                .texturePack = cPack.object,
                                                                .setBindingIndex = i
                                                            };

                                                            HF.renderingApi.api.BindTexturePack(handle, info);
                                                            descBindings[i] = cPack;
                                                        }
                                                        break;
                                                }
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

                if (rt.drawCallback) rt.drawCallback(rn, HF.renderingApi.api.GetCmd(handle));
                HF.renderingApi.api.EndRendering(handle);
            }
        }

        void BindBuffers(const void* handle, RenderPacket& packet,
            const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
        {
            const uint32_t bufferEnd = range.end();
            for (uint32_t bufferIndex = range.start; bufferIndex < bufferEnd; bufferIndex++)
            {
                const auto& bufferSet = packet.bufferSets.atC(bufferIndex);
                BufferBindInfo info
                {
                    .bindingType = bufferSet.bindingType,
                    .setBindingIndex = bufferSet.setBindingIndex,
                    .pBuffers = packet.buffers.atP(bufferSet.bufferRange.start),
                    .bufferCount = bufferSet.bufferRange.size
                };
                HF.renderingApi.api.BindBuffer(handle, info);
                descBindings[info.setBindingIndex] =
                {
                    .object = info.pBuffers,
                    .type = BUFFER
                };
            }
        }

        void CollectTexpacks(RenderPacket& packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
        {
            const uint32_t texpackEnd = range.end();
            for (uint32_t texpackIndex = range.start; texpackIndex < texpackEnd; texpackIndex++)
            {
                const auto texPack = packet.texpacks.atC(texpackIndex);
                descBindings[texPack.setBindingIndex] =
                {
                    .object = texPack.pack->handle,
                    .type = TEXPACK
                };
            }
        }

        void RendererDrawUploads(const void* handle, RenderPacket& packet)
        {
            //Uniform Buffer Uploads
            {
                const BufferUploadInfo uploadInfo
                {
                    .pUploadPackets = packet.bufferUploadPackets.begin(),
                    .uploadPacketCount = (uint16_t)packet.bufferUploadPackets.size(),
                    .pUniformDataBuffer = packet.bufferUploads.begin()
                };
                HF.renderingApi.api.UploadBuffer(handle, uploadInfo);
            }
            //Texture Pack Uploads.
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
                            packet.textures.atC(textures.range.start);

                            binding.texInfo = (TexturePackTextureUploadInfo)
                            {
                                .pTextures = (TexturePackTextureUploadInfo::TextureInfo*)packet.textures.atP(textures.range.start),
                                .offset = textures.offset,
                                .count = textures.range.size,
                            };
                        }

                        uploads[current] = binding;
                        current++;
                    }

                    HF.renderingApi.api.UploadTexturePackBinding(group.texturePack->handle, uploadInfo);
                }
            }
        }

        void DebugDraw_i(const Ref<Renderer>& rn)
        {
            // UploadStart_TexturePack(rn, HF.staticResources.gammaTexturePack);
            // {
            //     // UploadAdd_TexturePackBinding()
            // }
            // UploadEnd_TexturePack(rn);
            //
            // Start_RenderTexture(rn, HF.staticResources.debugRenderTexture);
            // {
            //     Start_ShaderSetup(rn, HF.staticResources.gammaCorrectionShaderSetup);
            //     {
            //         const hf::ShaderBindingInfo shaderInfo
            //         {
            //             .shader = HF.staticResources.gammaCorrectionShader,
            //             .attrib = HF.staticResources.quadAttrib,
            //             .bindingPoint = RenderBindingType::Graphics
            //         };
            //         Start_Shader(rn, shaderInfo);
            //         {
            //             Start_Material(rn, hf::primitives::GetEmptyMaterial());
            //             {
            //                 MaterialAdd_TexturePackBinding(rn, HF.staticResources.gammaTexturePack, 0);
            //                 Start_Draw(rn);
            //                 {
            //                     hf::DrawCallInfo drawInfo
            //                     {
            //                         .pVertBuffers = &HF.staticResources.quadBuffer,
            //                         .bufferCount = 1,
            //                         .instanceCount = 1
            //                     };
            //
            //                     DrawAdd_DrawCall(rn, drawInfo);
            //                 }
            //                 End_Draw(rn);
            //             }
            //             End_Material(rn);
            //         }
            //         End_Shader(rn);
            //     }
            //     End_ShaderSetup(rn);
            // }
            // End_RenderTexture(rn);
        }
    }
}
