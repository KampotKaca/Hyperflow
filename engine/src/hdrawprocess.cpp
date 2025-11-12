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
    namespace dp
    {
        void BindGlobalUniformBuffer()
        {
            StartBufferSet(RenderBindingType::Graphics, 0);
            BufferSetAddBuffer(ir::HF.staticResources.globalUniform);
            EndBufferSet();
        }

        void SetDrawCallback(void (*callback)(void*))
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentRenderTexture, "[Hyperflow] Cannot draw anything without render texture");
#endif
            ir::HF.renderer->currentDraw.currentRenderTexture->drawCallback = callback;
        }

        //region Scene Handling

        void SetCamera(const Camera3DAnchored& camera)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot set camera until drawing process starts");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->camera = Camera3DFreeLook
            {
                .position = camera.GetPosition(),
                .direction = camera.GetDirection(),
                .up = camera.up,
                .core = camera.core,
            };

            packet->camera.GetFrustum(packet->frustum);
        }

        void SetCamera(const Camera3DFreeLook& camera)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot set camera until drawing process starts");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->camera = camera;
            packet->camera.GetFrustum(packet->frustum);
        }

        void AddLight(const DirectionalLight& light)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot add light until drawing process starts");

            if (ir::HF.renderer->currentDraw.packet->directionalLights.size() >= MAX_DIRECTIONAL_LIGHTS)
            {
                log_error("Cannot add more than %i directional lights!", MAX_DIRECTIONAL_LIGHTS);
                return;
            }
#endif

            ir::HF.renderer->currentDraw.packet->directionalLights.push_back(light);
        }

        void AddLight(const SpotLight& light)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot add light until drawing process starts");

            if (ir::HF.renderer->currentDraw.packet->spotLights.size() >= MAX_SPOT_LIGHTS)
            {
                log_error("Cannot add more than %i spot lights!", MAX_SPOT_LIGHTS);
                return;
            }
#endif

            ir::HF.renderer->currentDraw.packet->spotLights.push_back(light);
        }

        void AddLight(const PointLight& light)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot add light until drawing process starts");

            if (ir::HF.renderer->currentDraw.packet->pointLights.size() >= MAX_POINT_LIGHTS)
            {
                log_error("Cannot add more than %i point lights!", MAX_POINT_LIGHTS);
                return;
            }
#endif

            ir::HF.renderer->currentDraw.packet->pointLights.push_back(light);
        }

        //endregion
        //region Uploads

        void UploadBuffer(const BufferUploadInfo& info)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot upload uniform until drawing process starts");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->bufferUploadPackets.push_back(BufferUploadPacketInfo
            {
                .buffer = info.buffer,
                .offsetInBytes = info.offsetInBytes,
                .bufferRange = AssetRange{ .start = (uint32_t)packet->bufferUploads.size(), .size = info.sizeInBytes }
            });

            packet->bufferUploads.insert(packet->bufferUploads.end(), (uint8_t*)info.data, (uint8_t*)info.data + info.sizeInBytes);
        }

        void UploadMat(const Ref<Material>& mat)
        {
            BufferUploadInfo info{};
            info.buffer = ir::HF.graphicsResources.materialDataStorageBuffer;
            info.offsetInBytes = mat->bufferIndex * RENDERING_MAX_MATERIAL_MEMORY_BADGET;
            info.sizeInBytes = mat->sizeInBytes;
            info.data = mat->bufferMemory;

            UploadBuffer(info);
        }

        void UploadStartTexPack(const Ref<TexturePack>& texPack)
        {
#if DEBUG
            hassert(ir::HF.renderer->isDrawing, "[Hyperflow] Cannot upload texture pack util drawing process starts");
            hassert(!ir::HF.renderer->currentDraw.currentTexturePackBinding, "[Hyperflow] Cannot start texture pack upload util you end previous one");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->textureGroupRebindings.push_back(TexturePackRebindingGroupPacketInfo
            {
                .texturePack = texPack,
                .bindingPacketRange = AssetRange{ .start = (uint16_t)packet->textureRebindings.size(), .size = (uint16_t)0 }
            });
            ir::HF.renderer->currentDraw.currentTexturePackBinding = &packet->textureGroupRebindings[packet->textureGroupRebindings.size() - 1];
        }

        void UploadEndTexPack()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentTexturePackBinding, "[Hyperflow] You need to start texture pack first");
#endif
            ir::HF.renderer->currentDraw.currentTexturePackBinding = nullptr;
        }

        template<typename T>
        static void UploadTex(SmallList<TexturePack::Binding<T>, MAX_TEXTURES_IN_TEXTURE_PACK>& bindings,
                              const TexturePackBindingUploadInfo<T>& info, TexturePackBindingType type)
        {
            const auto packet = ir::HF.renderer->currentDraw.packet;

            bool wasModified = false;
            auto binding = (TexturePackRebindingPacketInfo)
            {
                .bindingIndex = info.bindingIndex,
                .sampler = info.sampler,
            };

            if (info.sampler.has_value())
            {
                bindings[info.bindingIndex].sampler = info.sampler.value();
                wasModified = true;
            }

            if (info.texInfo.has_value())
            {
                auto textures = info.texInfo.value();
                for (uint32_t i = 0; i < textures.count; i++)
                {
                    auto& texInfo = textures.pTextures[i];
                    bindings[info.bindingIndex].textures[textures.offset + i] =
                    {
                        .texture = texInfo.texture,
                        .index = texInfo.index
                    };
                }

                binding.textures = (TexturePackRebindingPacketInfo::TextureBinding)
                {
                    .offset = textures.offset,
                    .type = type,
                    .range = AssetRange{ .start = (uint16_t)packet->textures.size(), .size = (uint16_t)textures.count }
                };

                for (uint32_t i = 0; i < textures.count; i++)
                {
                    auto& texture = textures.pTextures[i];
                    packet->textures.push_back({
                        .texture = texture.texture->handle,
                        .index = texture.index
                    });
                }
                if (textures.count > 0) wasModified = true;
            }

            if (wasModified)
            {
                packet->textureRebindings.push_back(binding);
                ir::HF.renderer->currentDraw.currentTexturePackBinding->bindingPacketRange.size++;
            }
            else log_warn_s("Unnecessary binding upload! nothing changed");
        }

        void UploadAddTexPackBinding(uint32_t bindingIndex, const Ref<Texture>& texture, uint32_t textureIndex)
        {
            TexturePackTextureUploadInfo<Texture>::TextureInfo texInfo{};
            texInfo.index = textureIndex;
            texInfo.texture = texture;

            TexturePackTextureUploadInfo<Texture> bindingTextureInfo{};
            bindingTextureInfo.offset = 0;
            bindingTextureInfo.count = 1;
            bindingTextureInfo.pTextures = &texInfo;

            TexturePackBindingUploadInfo<Texture> bindingInfo{};
            bindingInfo.bindingIndex = bindingIndex;
            bindingInfo.texInfo = bindingTextureInfo;

            UploadAddTexPackBindings(bindingInfo);
        }

        void UploadAddTexPackBinding(uint32_t bindingIndex, const Ref<Cubemap>& cubemap, uint32_t textureIndex)
        {
            TexturePackTextureUploadInfo<Cubemap>::TextureInfo texInfo{};
            texInfo.index = textureIndex;
            texInfo.texture = cubemap;

            TexturePackTextureUploadInfo<Cubemap> bindingTextureInfo{};
            bindingTextureInfo.offset = 0;
            bindingTextureInfo.count = 1;
            bindingTextureInfo.pTextures = &texInfo;

            TexturePackBindingUploadInfo<Cubemap> bindingInfo{};
            bindingInfo.bindingIndex = bindingIndex;
            bindingInfo.texInfo = bindingTextureInfo;

            UploadAddTexPackBindings(bindingInfo);
        }

        void UploadAddTexPackBinding(uint32_t bindingIndex, const Ref<RenderTexture>& rt, uint32_t attachmentIndex)
        {
            TexturePackTextureUploadInfo<RenderTexture>::TextureInfo texInfo{};
            texInfo.index = attachmentIndex;
            texInfo.texture = rt;

            TexturePackTextureUploadInfo<RenderTexture> bindingTextureInfo{};
            bindingTextureInfo.offset = 0;
            bindingTextureInfo.count = 1;
            bindingTextureInfo.pTextures = &texInfo;

            TexturePackBindingUploadInfo<RenderTexture> bindingInfo{};
            bindingInfo.bindingIndex = bindingIndex;
            bindingInfo.texInfo = bindingTextureInfo;

            UploadAddTexPackBindings(bindingInfo);
        }

        void UploadAddTexPackBindings(const TexturePackBindingUploadInfo<Texture>& info)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentTexturePackBinding, "[Hyperflow] Cannot add binding packet without starting texture pack upload");
#endif

            UploadTex(ir::HF.renderer->currentDraw.currentTexturePackBinding->texturePack->textureBindings, info, TexturePackBindingType::Texture2D);
        }

        void UploadAddTexPackBindings(const TexturePackBindingUploadInfo<Cubemap>& info)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentTexturePackBinding, "[Hyperflow] Cannot add binding packet without starting texture pack upload");
#endif

            UploadTex(ir::HF.renderer->currentDraw.currentTexturePackBinding->texturePack->cubemapBindings, info, TexturePackBindingType::Cubemap);
        }

        void UploadAddTexPackBindings(const TexturePackBindingUploadInfo<RenderTexture>& info)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentTexturePackBinding, "[Hyperflow] Cannot add binding packet without starting texture pack upload");
#endif

            UploadTex(ir::HF.renderer->currentDraw.currentTexturePackBinding->texturePack->renderTextureBindings, info, TexturePackBindingType::RenderTexture);
        }

        //endregion
        //region Render Texture

        void AddRenderTexDependency(const RenderAttachmentDependencyInfo& info)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentRenderTexture, "[Hyperflow] Can't have any dependencies without any render texture!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->dependencies.push_back(info);
            ir::HF.renderer->currentDraw.currentRenderTexture->dependencyRange.size++;
        }

        void StartRenderTex(const Ref<RenderTexture>& rt)
        {
#if DEBUG
            hassert(!ir::HF.renderer->currentDraw.currentRenderTexture, "[Hyperflow] You are already drawing on render texture!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->renderTextures.push_back(RenderTexturePacketInfo
            {
                .texture = rt,
                .shaderLayoutRange = AssetRange{ .start = (uint16_t)packet->shaderLayouts.size(), .size = (uint16_t)0 },
                .dependencyRange = AssetRange{ .start = (uint8_t)packet->dependencies.size(), .size = (uint8_t)0 }
            });

            ir::HF.renderer->currentDraw.currentRenderTexture = &packet->renderTextures[packet->renderTextures.size() - 1];
        }

        void EndRenderTex()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentRenderTexture, "[Hyperflow] Can't end render texture drawing without starting it!");
#endif

            ir::HF.renderer->currentDraw.currentRenderTexture = nullptr;
        }

        //endregion
        //region Shader

        void StartShaderLayout(ShaderLayout layout)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentRenderTexture, "[Hyperflow] Cannot draw anything without starting render texture!");
            hassert(!ir::HF.renderer->currentDraw.currentShaderLayout, "[Hyperflow] Cannot start ShaderLayout without ending previous one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->shaderLayouts.push_back(ShaderLayoutPacketInfo
            {
                .layout = layout,
                .shaderPacketRange = AssetRange{ .start = (uint16_t)packet->shaders.size(), .size = (uint16_t)0 },
                .bufferSetRange = AssetRange{ .start = (uint32_t)packet->bufferSets.size(), .size = (uint32_t)0 },
            });

            ir::HF.renderer->currentDraw.currentShaderLayout = &packet->shaderLayouts[packet->shaderLayouts.size() - 1];
            ir::HF.renderer->currentDraw.currentRenderTexture->shaderLayoutRange.size++;
        }

        void EndShaderLayout()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShaderLayout, "[Hyperflow] Cannot end ShaderLayout without starting it!");
#endif

            if (ir::HF.renderer->currentDraw.currentShaderLayout->shaderPacketRange.size == 0)
            {
                ir::HF.renderer->currentDraw.currentRenderTexture->shaderLayoutRange.size--;
                ir::HF.renderer->currentDraw.packet->shaderLayouts.pop_back();
            }

            ir::HF.renderer->currentDraw.currentShaderLayout = nullptr;
        }

        void StartShader(const Ref<Shader>& shader)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShaderLayout, "[Hyperflow] ShaderLayout must be set!");
            hassert(!ir::HF.renderer->currentDraw.currentShader, "[Hyperflow] Cannot start Shader without ending previous one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->shaders.push_back(ShaderPacketInfo
            {
                .shader = shader,
                .materialPacketRange = AssetRange{ .start = (uint16_t)packet->materials.size(), .size = (uint16_t)0 }
            });

            ir::HF.renderer->currentDraw.currentShader = &packet->shaders[packet->shaders.size() - 1];
            ir::HF.renderer->currentDraw.currentShaderLayout->shaderPacketRange.size++;
        }

        void EndShader()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShaderLayout, "[Hyperflow] ShaderLayout must be set!");
            hassert(ir::HF.renderer->currentDraw.currentShader, "[Hyperflow] Cannot end Shader without starting it!");
#endif

            if (ir::HF.renderer->currentDraw.currentShader->materialPacketRange.size == 0)
            {
                ir::HF.renderer->currentDraw.currentShaderLayout->shaderPacketRange.size--;
                ir::HF.renderer->currentDraw.packet->shaders.pop_back();
            }

            ir::HF.renderer->currentDraw.currentShader = nullptr;
        }

        //endregion
        //region Material

        void StartMat(const Ref<Material>& material)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShader, "[Hyperflow] Shader must be set!");
            hassert(!ir::HF.renderer->currentDraw.currentMaterial, "[Hyperflow] Cannot start Material without ending previous one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->materials.push_back(MaterialPacketInfo
            {
                .material = material,
                .drawGroupRange = AssetRange{ .start = (uint32_t)packet->drawPackets.size(), .size = (uint32_t)0 },
                .texpackRange = AssetRange{ .start = (uint32_t)packet->texpacks.size(), .size = (uint32_t)0 },
            });

            ir::HF.renderer->currentDraw.currentMaterial = &packet->materials[packet->materials.size() - 1];
            ir::HF.renderer->currentDraw.currentShader->materialPacketRange.size++;
        }

        void EndMat()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShader, "[Hyperflow] Shader must be set!");
            hassert(ir::HF.renderer->currentDraw.currentMaterial, "[Hyperflow] Cannot end material without starting it!");
#endif

            if (ir::HF.renderer->currentDraw.currentMaterial->drawGroupRange.size == 0)
            {
                ir::HF.renderer->currentDraw.currentShader->materialPacketRange.size--;
                ir::HF.renderer->currentDraw.packet->materials.pop_back();
            }

            ir::HF.renderer->currentDraw.currentMaterial = nullptr;
        }

        void MatAddTexPackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentMaterial, "[Hyperflow] Material must be set!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            TexPackBindingInfo bindingInfo{};
            bindingInfo.pack = texPack;
            bindingInfo.setBindingIndex = setBindingIndex;

            packet->texpacks.push_back(bindingInfo);
            ir::HF.renderer->currentDraw.currentMaterial->texpackRange.size++;
        }

        //endregion
        //region Draw Group

        void StartDrawGroup()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentMaterial, "[Hyperflow] Material must be set!");
            hassert(!ir::HF.renderer->currentDraw.currentDrawPacket, "[Hyperflow] Need to end draw before starting new one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->drawPackets.push_back(DrawPacketInfo
            {
                .texpackRange = AssetRange{ .start = (uint32_t)packet->texpacks.size(), .size = (uint32_t)0 },
                .drawCallRange = AssetRange{ .start = (uint32_t)packet->drawCalls.size(), .size = (uint32_t)0 },
                .pushConstantRange = AssetRange{ .start = (uint32_t)packet->pushConstantUploads.size(), .size = (uint32_t)0 },
            });

            ir::HF.renderer->currentDraw.currentDrawPacket = &packet->drawPackets[packet->drawPackets.size() - 1];
            ir::HF.renderer->currentDraw.currentMaterial->drawGroupRange.size++;
        }

        void EndDrawGroup()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShader, "[Hyperflow] Shader must be set!");
            hassert(ir::HF.renderer->currentDraw.currentDrawPacket, "[Hyperflow] Need to start draw before ending it!");
#endif

            if (ir::HF.renderer->currentDraw.currentDrawPacket->drawCallRange.size == 0)
            {
                ir::HF.renderer->currentDraw.currentMaterial->drawGroupRange.size--;
                ir::HF.renderer->currentDraw.packet->drawPackets.pop_back();
            }

            ir::HF.renderer->currentDraw.currentDrawPacket = nullptr;
        }

        void DrawGroupAddTexPackBinding(const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentDrawPacket, "[Hyperflow] DrawPacket must be set!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;

            TexPackBindingInfo bindingInfo{};
            bindingInfo.pack = texPack;
            bindingInfo.setBindingIndex = setBindingIndex;

            packet->texpacks.push_back(bindingInfo);
            ir::HF.renderer->currentDraw.currentDrawPacket->texpackRange.size++;
        }

        void DrawGroupAddDrawCall(const Ref<IndexBuffer>& indexBuffer, const Ref<VertexBuffer>& vertexBuffer)
        {
            StartDrawCall(indexBuffer);
            DrawAddVertBuffer(vertexBuffer);

            ir::HF.renderer->currentDraw.currentDrawCall->isInstanced = false;

            EndDrawCall();
        }

        void DrawGroupAddDrawCall(const Ref<Mesh>& mesh)
        {
            StartDrawCall(mesh);
            ir::HF.renderer->currentDraw.currentDrawCall->isInstanced = false;
            EndDrawCall();
        }

        void DrawGroupSetPushConstant(const void* data, uint32_t dataSize)
        {
            const auto& currentDraw = ir::HF.renderer->currentDraw;
#if DEBUG
            hassert(currentDraw.currentDrawPacket, "[Hyperflow] DrawPacket must be set!");
            hassert(currentDraw.currentDrawPacket->pushConstantRange.size == 0, "[Hyperflow] Cannot set push constant twice for a single draw packet!");
#endif

            const auto packet = currentDraw.packet;
            currentDraw.currentDrawPacket->pushConstantRange.size = dataSize;
            packet->pushConstantUploads.insert(packet->pushConstantUploads.end(), (uint8_t*)data, (uint8_t*)data + dataSize);
        }

        //endregion
        //region Buffer Set

        void StartBufferSet(RenderBindingType bindingType, uint32_t setBindingIndex)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentShaderLayout, "[Hyperflow] Shader Setup must be set until you set uniform set");
            hassert(!ir::HF.renderer->currentDraw.currentUniformSet, "[Hyperflow] Cannot start uniform set without ending previous one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->bufferSets.push_back(BufferSetPacketInfo
            {
                .bindingType = bindingType,
                .setBindingIndex = setBindingIndex,
                .bufferRange = AssetRange{ .start = (uint16_t)packet->buffers.size(), .size = (uint16_t)0 },
            });
            ir::HF.renderer->currentDraw.currentUniformSet = &packet->bufferSets[packet->bufferSets.size() - 1];
            ir::HF.renderer->currentDraw.currentShaderLayout->bufferSetRange.size++;
        }

        void EndBufferSet()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentUniformSet, "[Hyperflow] Cannot end uniform set without starting it!");
#endif
            ir::HF.renderer->currentDraw.currentUniformSet = nullptr;
        }

        void BufferSetAddBuffer(Buffer buffer)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentUniformSet, "[Hyperflow] Uniform Set must be set!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            packet->buffers.push_back(buffer);
            ir::HF.renderer->currentDraw.currentUniformSet->bufferRange.size++;
        }

        //endregion
        //region DrawCall

        void StartDrawCall(const Ref<IndexBuffer>& indexBuffer)
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentDrawPacket, "[Hyperflow] DrawPacket must be set!");
            hassert(!ir::HF.renderer->currentDraw.currentDrawCall, "[Hyperflow] Cannot start draw call without ending previous one!");
#endif

            const auto packet = ir::HF.renderer->currentDraw.packet;
            DrawCallPacketInfo packetInfo{};
            packetInfo.vertexBufferRange = AssetRange{ .start = (uint32_t)packet->vertexBuffers.size(), .size = (uint32_t)0 };
            packetInfo.instanceRange = AssetRange{ .start = (uint32_t)packet->instances.size(), .size = (uint32_t)0 };
            packetInfo.indexBuffer = indexBuffer;
            packetInfo.instanceBufferOffset = (uint32_t)packet->instanceUploads.size();

            packet->drawCalls.push_back(packetInfo);
            ir::HF.renderer->currentDraw.currentDrawPacket->drawCallRange.size++;
            ir::HF.renderer->currentDraw.currentDrawCall = &packet->drawCalls[packet->drawCalls.size() - 1];
        }

        void StartDrawCall(const Ref<Mesh>& mesh)
        {
            StartDrawCall(mesh->indexBuffer);
            DrawAddVertBuffer(mesh->vertBuffer);
#if DEBUG
            strcpy(ir::HF.renderer->currentDraw.currentDrawCall->debugName, mesh->name.c_str());
#endif
        }

        void StartDrawCall(const Ref<Mesh>& mesh, const Ref<Armature>& armature, uint32_t skinIndex)
        {
            StartDrawCall(mesh->indexBuffer);
            DrawAddVertBuffer(mesh->vertBuffer);

            if (skinIndex >= 0)
            {
                hassert(mesh->skinBuffers.size() > skinIndex, "[Hyperflow] Skin index is out of bounds!");
                DrawAddVertBuffer(mesh->skinBuffers[skinIndex]);
            }
#if DEBUG
            strcpy(ir::HF.renderer->currentDraw.currentDrawCall->debugName, mesh->name.c_str());
#endif
        }

        void EndDrawCall()
        {
#if DEBUG
            hassert(ir::HF.renderer->currentDraw.currentDrawCall, "[Hyperflow] Cannot start draw call without ending previous one!");
#endif

            if (ir::HF.renderer->currentDraw.currentDrawCall->isInstanced && ir::HF.renderer->currentDraw.currentDrawCall->instanceRange.size == 0)
            {
                ir::HF.renderer->currentDraw.currentDrawPacket->drawCallRange.size--;
                ir::HF.renderer->currentDraw.packet->drawCalls.pop_back();
            }

            ir::HF.renderer->currentDraw.currentDrawCall = nullptr;
        }

        void DrawAddInstance(const void* data, uint32_t dataSize, const VolumeTransform& volume)
        {
            const auto& currentDraw = ir::HF.renderer->currentDraw;
#if DEBUG
            hassert(currentDraw.currentDrawPacket, "[Hyperflow] DrawPacket must be set!");
            hassert(dataSize <= MAX_PER_INSTANCE_BUFFER_SIZE,
                    "[Hyperflow] Instance data cannot be more the %i bytes!", MAX_PER_INSTANCE_BUFFER_SIZE);
            hassert(currentDraw.packet->instanceUploads.size() + dataSize <= MAX_INSTANCE_BUFFER_SIZE_BYTES,
                    "[Hyperflow] Too much instance data, max size is %i bytes!", MAX_INSTANCE_BUFFER_SIZE_BYTES);
            hassert(currentDraw.currentDrawCall->singleInstanceSize == 0 ||
                    currentDraw.currentDrawCall->singleInstanceSize == dataSize,
                    "[Hyperflow] Instance data size must be the same for all instances!");
#endif

            const auto packet = currentDraw.packet;

            if (!packet->frustum.IsVisible(volume.worldVolume))
            {
                packet->statistics.culledDrawCalls++;
                return;
            }

            currentDraw.currentDrawCall->instanceRange.size++;
            currentDraw.currentDrawCall->singleInstanceSize = dataSize;

            packet->instances.push_back(InstancePacketInfo{});
            packet->instanceUploads.insert(packet->instanceUploads.end(), (uint8_t*)data, (uint8_t*)data + dataSize);
        }

        void DrawAddVertBuffer(const Ref<VertexBuffer>& vb)
        {
            const auto& currentDraw = ir::HF.renderer->currentDraw;
#if DEBUG
            hassert(currentDraw.currentDrawCall, "[Hyperflow] You must be start draw call first!");
#endif

            const auto packet = currentDraw.packet;
            packet->vertexBuffers.push_back(vb);
            currentDraw.currentDrawCall->vertexBufferRange.size++;
        }

        //endregion
    }

    namespace ir::rdr
    {
        void StartRenderPacket_i()
        {
#if DEBUG
            hassert(!HF.renderer->currentDraw.packet, "[Hyperflow] Cannot start RenderPacket, without submitting previous one!");
#endif

            std::lock_guard lock(HF.renderer->threadInfo.threadLock);
            HF.renderer->currentDraw.packet = HF.renderer->threadInfo.cachedPackets.back();
            HF.renderer->threadInfo.cachedPackets.pop_back();
            HF.renderer->currentDraw.packet->clear();
            HF.renderer->isDrawing = true;
        }

        void EndRenderPacket_i()
        {
#if DEBUG
            hassert(HF.renderer->isDrawing, "[Hyperflow] Cannot end RenderPacket, without starting it!");
#endif

            {
                const auto packet = HF.renderer->currentDraw.packet;
                HF.renderer->lastDrawStatistics = packet->statistics;

                GlobalUniformInfo uniformInfo{};
                uniformInfo.camera = packet->camera.GetUniformInfo();
                uniformInfo.time = HF.time.GetUniformInfo();

                for (uint32_t i = 0; i < packet->directionalLights.size(); i++)
                    uniformInfo.light.directionalLights[i] = packet->directionalLights[i].GetUniformInfo();

                for (uint32_t i = 0; i < packet->spotLights.size(); i++)
                    uniformInfo.light.spotLights[i] = packet->spotLights[i].GetUniformInfo();

                for (uint32_t i = 0; i < packet->pointLights.size(); i++)
                    uniformInfo.light.pointLights[i] = packet->pointLights[i].GetUniformInfo();

                uniformInfo.light.lightCounts = uvec3
                {
                    (uint32_t)packet->directionalLights.size(),
                    (uint32_t)packet->spotLights.size(),
                    (uint32_t)packet->pointLights.size()
                };

                BufferUploadInfo uniformUpload{};
                uniformUpload.buffer = HF.staticResources.globalUniform;
                uniformUpload.offsetInBytes = 0;
                uniformUpload.sizeInBytes = sizeof(GlobalUniformInfo);
                uniformUpload.data = &uniformInfo;

                dp::UploadBuffer(uniformUpload);
            }

            {
                std::lock_guard lock(HF.renderer->threadInfo.threadLock);;

                if (HF.renderer->threadInfo.drawPacket) HF.renderer->threadInfo.cachedPackets.push_back(HF.renderer->threadInfo.drawPacket);
                HF.renderer->threadInfo.drawPacket = HF.renderer->currentDraw.packet;
                HF.renderer->currentDraw.packet = nullptr;
                HF.renderer->threadInfo.renderCondition.notify_one();
            }

            HF.renderer->currentDraw.clear();
            HF.renderer->isDrawing = false;
        }

        void PreDraw_i()
        {
            static List<uint64_t> deletedMaterials{};
            deletedMaterials.clear();
            for (auto& material : std::views::values(HF.graphicsResources.materials))
            {
                if (material.use_count() == 1) deletedMaterials.push_back((uint64_t)material.get());
                else
                {
                    if (material->isMarkedAsModified)
                    {
                        dp::UploadMat(material);
                        material->isMarkedAsModified = false;
                    }
                }
            }

            for (auto& material : deletedMaterials) HF.graphicsResources.materials.erase(material);
        }

        void RendererUpdate_i()
        {
            HF.renderingApi.api.WaitForPreviousFrame(HF.renderer->handle);
            CleanMarkedResources_i();

            auto& tInfo = HF.renderer->threadInfo;
            RenderPacket* packet;
            {
                std::unique_lock lock(tInfo.threadLock);
                tInfo.renderCondition.wait(lock, [&]{ return tInfo.drawPacket || !tInfo.isRunning; });
                if (!tInfo.isRunning) return;
                packet = tInfo.drawPacket;
                tInfo.drawPacket = nullptr;
            }

            void* renderTextures[RENDERING_MAX_NUM_RENDER_TEXTURES];
            for (uint8_t rtIndex = 0; rtIndex < (uint8_t)packet->renderTextures.size(); rtIndex++)
                renderTextures[rtIndex] = packet->renderTextures[rtIndex].texture->handle;

            uvec2 size = HF.renderingApi.api.GetReadyForRendering(HF.renderer->handle, renderTextures, packet->renderTextures.size());
            if (size != uvec2(0, 0))
            {
                for (const auto& rtPacket : packet->renderTextures) rtPacket.texture->createInfo.size = size;

                HF.renderingApi.api.StartFrame(HF.renderer->handle);
                RendererDraw_i(packet);
                HF.renderingApi.api.EndFrame(HF.renderer->handle);
            }

            std::lock_guard lock(tInfo.threadLock);
            tInfo.cachedPackets.push_back(packet);
        }

        static void RendererDrawUploads(const void* handle, RenderPacket* packet);
        static void BindBuffers(const void* handle, const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);
        static void CollectTexPacks(const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);

        void RendererDraw_i(RenderPacket* packet)
        {
            const auto handle = HF.renderer->handle;
            RendererDrawUploads(handle, packet);

            for (uint8_t rtIndex = 0; rtIndex < (uint8_t)packet->renderTextures.size(); rtIndex++)
            {
                auto& rt = packet->renderTextures[rtIndex];
                if (rt.dependencyRange.size > 0)
                {
                    HF.renderingApi.api.ApplyRenderAttachmentDependencies(handle,
                    &packet->dependencies[rt.dependencyRange.start], rt.dependencyRange.size);
                }

                HF.renderingApi.api.BeginRendering(handle, rt.texture->handle);

                const uint16_t shaderLayoutEnd = rt.shaderLayoutRange.end();
                for (uint16_t shaderLayoutIndex = rt.shaderLayoutRange.start; shaderLayoutIndex < shaderLayoutEnd; shaderLayoutIndex++)
                {
                    DescriptorBindingInfo descBindings[MAX_NUM_BOUND_DESCRIPTORS]{};

                    const auto& shaderLayout = packet->shaderLayouts[shaderLayoutIndex];
                    HF.renderingApi.api.BindShaderLayout(handle, shaderLayout.layout);

                    BindBuffers(handle, packet, shaderLayout.bufferSetRange, descBindings);

                    const uint16_t shaderEnd = shaderLayout.shaderPacketRange.end();
                    for (uint16_t shaderIndex = shaderLayout.shaderPacketRange.start; shaderIndex < shaderEnd; shaderIndex++)
                    {
                        const auto& shader = packet->shaders[shaderIndex];
                        const auto& shaderHandle = shader.shader->handle;

                        if (shaderHandle)
                        {
                            HF.renderingApi.api.BindShader(handle, shaderHandle);

                            const uint16_t materialEnd = shader.materialPacketRange.end();
                            for (uint16_t materialIndex = shader.materialPacketRange.start; materialIndex < materialEnd; materialIndex++)
                            {
                                auto& material = packet->materials[materialIndex];

                                if (material.material && material.material->sizeInBytes > 0)
                                {
                                    BindResourceInfo_i<Buffer> info{};
                                    info.bindingType = RenderBindingType::Graphics;
                                    info.setBindingIndex = 1;
                                    info.objectCount = 1;

                                    info.objects[0] = HF.graphicsResources.materialDataStorageBuffer,

                                    HF.renderingApi.api.BindBuffer(handle, info);
                                }

                                const uint32_t drawPacketEnd = material.drawGroupRange.end();
                                for (uint32_t drawPacketIndex = material.drawGroupRange.start; drawPacketIndex < drawPacketEnd; drawPacketIndex++)
                                {
                                    auto& drawPacket = packet->drawPackets[drawPacketIndex];

                                    if (drawPacket.pushConstantRange.size > 0)
                                    {
                                        PushConstantUploadInfo uploadInfo{};
                                        uploadInfo.layout = shaderLayout.layout;
                                        uploadInfo.data = &packet->pushConstantUploads[drawPacket.pushConstantRange.start];

                                        HF.renderingApi.api.UploadPushConstants(handle, uploadInfo);
                                    }

                                    {// Binding Texture packs
                                        DescriptorBindingInfo currentDescriptors[MAX_NUM_BOUND_DESCRIPTORS]{};
                                        CollectTexPacks(packet, material.texpackRange, currentDescriptors);
                                        CollectTexPacks(packet, drawPacket.texpackRange, currentDescriptors);

                                        for (uint32_t i = 0; i < MAX_NUM_BOUND_DESCRIPTORS; i++)
                                        {
                                            auto cPack = currentDescriptors[i];
                                            if (cPack.object != descBindings[i].object && cPack.object)
                                            {
                                                switch (cPack.type)
                                                {
                                                    case BUFFER: log_error("[Hyperflow] Cannot bind buffer here!");
                                                    case TEXPACK:
                                                    {
                                                        BindResourceInfo_i<void*> info{};
                                                        info.bindingType = RenderBindingType::Graphics;
                                                        info.setBindingIndex = i;
                                                        info.objectCount = 1;

                                                        info.objects[0] = cPack.object;

                                                        HF.renderingApi.api.BindTexturePack(handle, info);
                                                        descBindings[i] = cPack;
                                                    }break;
                                                }
                                            }
                                        }
                                    }

                                    const uint32_t drawCallEnd = drawPacket.drawCallRange.end();
                                    for (uint32_t drawCallIndex = drawPacket.drawCallRange.start; drawCallIndex < drawCallEnd; drawCallIndex++)
                                    {
                                        const auto& drawCall = packet->drawCalls[drawCallIndex];
#if DEBUG
                                        hassert(drawCall.vertexBufferRange.size <= MAX_NUM_BUFFER_CACHE, "[Hyperflow] Trying to draw too many buffers at once, max is %i", MAX_NUM_BUFFER_CACHE);
#endif

                                        uint32_t bufferCount = drawCall.vertexBufferRange.size;

                                        DrawCallInfo_i drawInfo{};
                                        drawInfo.vertexBufferCount = bufferCount;

                                        {
                                            uint32_t currentIndex = 0;
                                            const uint32_t vertexBufferEnd = drawCall.vertexBufferRange.end();
                                            for (uint32_t i = drawCall.vertexBufferRange.start; i < vertexBufferEnd; i++)
                                            {
                                                drawInfo.pVertexBuffers[currentIndex] = VertexBufferRegionInfo_i
                                                {
                                                    .buffer = packet->vertexBuffers[i]->handle,
                                                    .offsetInBytes = 0
                                                };
                                                currentIndex++;
                                            }
                                        }

                                        if (drawCall.instanceRange.size > 0)
                                        {
                                            drawInfo.instanceBuffer = InstanceBufferRegionInfo_i
                                            {
                                                .buffer = HF.staticResources.instanceBuffer->handle,
                                                .offsetInBytes = drawCall.instanceBufferOffset,
                                                .instanceCount = drawCall.instanceRange.size
                                            };
                                        }

                                        if (drawCall.indexBuffer)
                                        {
                                            IndexedDrawCallInfo_i idcInfo{};
                                            idcInfo.drawInfo = drawInfo;
                                            idcInfo.indexBuffer = IndexBufferRegionInfo_i
                                            {
                                                .buffer = drawCall.indexBuffer->handle,
                                                .offset = 0,
                                                .indexCount = drawCall.indexBuffer->details.indexCount
                                            };

                                            HF.renderingApi.api.DrawIndexed(handle, idcInfo);
                                        }
                                        else
                                        {
                                            VertexedDrawCallInfo_i vdcInfo{};
                                            vdcInfo.drawInfo = drawInfo;
                                            vdcInfo.vertexCount = packet->vertexBuffers[drawCall.vertexBufferRange.start]->vertexCount;

                                            HF.renderingApi.api.Draw(handle, vdcInfo);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (rt.drawCallback) rt.drawCallback(HF.renderingApi.api.GetCmd(handle));
                HF.renderingApi.api.EndRendering(handle);
            }
        }

        void BindBuffers(const void* handle, const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
        {
            const uint32_t bufferEnd = range.end();
            for (uint32_t bufferIndex = range.start; bufferIndex < bufferEnd; bufferIndex++)
            {
                const auto& bufferSet = packet->bufferSets[bufferIndex];

                BindResourceInfo_i<Buffer> info{};
                info.bindingType = bufferSet.bindingType;
                info.setBindingIndex = bufferSet.setBindingIndex;
                info.objectCount = bufferSet.bufferRange.size;

                const auto end = bufferSet.bufferRange.end();
                uint32_t index = 0;
                for (uint32_t i = bufferSet.bufferRange.start; i < end; i++)
                {
                    info.objects[index] = packet->buffers.at(i);

                    DescriptorBindingInfo dbi{};
                    dbi.object = (void*)(uint64_t)info.objects[index];
                    dbi.type = BUFFER;

                    descBindings[info.setBindingIndex] = dbi;
                    index++;
                }

                HF.renderingApi.api.BindBuffer(handle, info);
            }
        }

        void CollectTexPacks(const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
        {
            const uint32_t texpackEnd = range.end();
            for (uint32_t texpackIndex = range.start; texpackIndex < texpackEnd; texpackIndex++)
            {
                const auto texPack = packet->texpacks[texpackIndex];
                descBindings[texPack.setBindingIndex] = DescriptorBindingInfo
                {
                    .object = texPack.pack->handle,
                    .type = TEXPACK
                };
            }
        }

        void RendererDrawUploads(const void* handle, RenderPacket* packet)
        {
            //Uniform Buffer Uploads
            if (packet->bufferUploadPackets.size() > 0)
            {
                BufferUploadInfo_i uploadInfo{};
                uploadInfo.pUploadPackets = &packet->bufferUploadPackets[0];
                uploadInfo.uploadPacketCount = (uint16_t)packet->bufferUploadPackets.size();
                uploadInfo.data = &packet->bufferUploads[0];

                HF.renderingApi.api.UploadBuffer(handle, uploadInfo);
            }

            //Instance Buffer Uploads
            if (packet->instanceUploads.size() > 0)
            {
                VertexBufferUploadInfo_i vbUploadInfo{};
                vbUploadInfo.buffer = HF.staticResources.instanceBuffer->handle;
                vbUploadInfo.data = &packet->instanceUploads[0];
                vbUploadInfo.offsetInBytes = 0;
                vbUploadInfo.sizeInBytes = packet->instanceUploads.size();

                HF.renderingApi.api.UploadVertexBuffer(handle, vbUploadInfo);
            }

            //Texture Pack Uploads.
            {
                static TexturePackBindingUploadInfo_i uploads[128];
                for (uint32_t i = 0; i < packet->textureGroupRebindings.size(); i++)
                {
                    const auto& group = packet->textureGroupRebindings[i];

                    TexturePackBindingUploadGroupInfo_i uploadInfo{};
                    uploadInfo.bindings = uploads;
                    uploadInfo.bindingCount = group.bindingPacketRange.size;

                    uint32_t current = 0;
                    const uint32_t end = group.bindingPacketRange.end();
                    for (uint32_t j = group.bindingPacketRange.start; j < end; j++)
                    {
                        auto& p = packet->textureRebindings[j];

                        TexturePackBindingUploadInfo_i binding{};
                        binding.bindingIndex = p.bindingIndex;
                        binding.sampler      = p.sampler;

                        if (p.textures.has_value())
                        {
                            const auto& textures = p.textures.value();

                            TexturePackTextureUploadInfo_i tuInfo{};
                            tuInfo.pTextures = (TexturePackTextureUploadInfo_i::TextureInfo*)&packet->textures[textures.range.start];
                            tuInfo.offset = textures.offset;
                            tuInfo.count = textures.range.size;

                            binding.texInfo = tuInfo;
                        }

                        uploads[current] = binding;
                        current++;
                    }

                    HF.renderingApi.api.UploadTexturePackBinding(group.texturePack->handle, uploadInfo);
                }
            }
        }
    }
}
