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

    void Set_Camera(const Ref<Renderer>& rn, const Camera3DAnchored& camera)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot set camera until drawing process starts");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->camera = Camera3DFreeLook
            {
                .position = camera.GetPosition(),
                .direction = camera.GetDirection(),
                .up = camera.up,
                .core = camera.core,
            };

            packet->camera.GetFrustum(rn, packet->frustum);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to set camera!");
            throw;
        }
    }

    void Set_Camera(const Ref<Renderer>& rn, const Camera3DFreeLook& camera)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot set camera until drawing process starts");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->camera = camera;
            packet->camera.GetFrustum(rn, packet->frustum);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to set camera!");
            throw;
        }
    }

    void Add_Light(const Ref<Renderer>& rn, const DirectionalLight& light)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add light until drawing process starts");

            if (rn->currentDraw.packet->directionalLights.size() >= MAX_DIRECTIONAL_LIGHTS)
            {
                LOG_ERROR("Cannot add more than %i directional lights!", MAX_DIRECTIONAL_LIGHTS);
                return;
            }
#endif

            rn->currentDraw.packet->directionalLights.push_back(light);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add light!");
            throw;
        }
    }

    void Add_Light(const Ref<Renderer>& rn, const SpotLight& light)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add light until drawing process starts");

            if (rn->currentDraw.packet->spotLights.size() >= MAX_SPOT_LIGHTS)
            {
                LOG_ERROR("Cannot add more than %i spot lights!", MAX_SPOT_LIGHTS);
                return;
            }
#endif

            rn->currentDraw.packet->spotLights.push_back(light);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add light!");
            throw;
        }
    }

    void Add_Light(const Ref<Renderer>& rn, const PointLight& light)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot add light until drawing process starts");

            if (rn->currentDraw.packet->pointLights.size() >= MAX_POINT_LIGHTS)
            {
                LOG_ERROR("Cannot add more than %i point lights!", MAX_POINT_LIGHTS);
                return;
            }
#endif

            rn->currentDraw.packet->pointLights.push_back(light);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to add light!");
            throw;
        }
    }

    void Upload_Buffer(const Ref<Renderer>& rn, const BufferUploadInfo& info)
    {
        try
        {
#if DEBUG
            if (!rn->isDrawing)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot upload uniform until drawing process starts");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->bufferUploadPackets.push_back(BufferUploadPacketInfo
            {
                .buffer = info.buffer,
                .offsetInBytes = info.offsetInBytes,
                .bufferRange = AssetRange{ .start = (uint32_t)packet->bufferUploads.size(), .size = info.sizeInBytes }
            });

            packet->bufferUploads.insert(packet->bufferUploads.end(),
                             (uint8_t*)info.data,
                             (uint8_t*)info.data + info.sizeInBytes);
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
            BufferUploadInfo info{};
            info.buffer = inter::HF.graphicsResources.materialDataStorageBuffer;
            info.offsetInBytes = material->bufferIndex * RENDERING_MAX_MATERIAL_MEMORY_BADGET;
            info.sizeInBytes = material->sizeInBytes;
            info.data = material->bufferMemory;

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

            const auto packet = rn->currentDraw.packet;
            packet->textureGroupRebindings.push_back(TexturePackRebindingGroupPacketInfo
            {
                .texturePack = texPack,
                .bindingPacketRange = AssetRange{ .start = (uint16_t)packet->textureRebindings.size(), .size = (uint16_t)0 }
            });
            rn->currentDraw.currentTexturePackBinding = &packet->textureGroupRebindings[packet->textureGroupRebindings.size() - 1];
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
        const auto packet = rn->currentDraw.packet;
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
                .range = AssetRange{ .start = (uint16_t)packet->textures.size(), .size = (uint16_t)textures.count }
            };

            for (uint32_t i = 0; i < textures.count; i++)
            {
                auto texture = textures.pTextures[i];
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
            rn->currentDraw.currentTexturePackBinding->bindingPacketRange.size++;
        }
        else LOG_WARN("Unnecessary binding upload! nothing changed");
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<Texture>& texture, uint32_t textureIndex)
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

        UploadAdd_TexturePackBindings(rn, bindingInfo);
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<Cubemap>& cubemap, uint32_t textureIndex)
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

        UploadAdd_TexturePackBindings(rn, bindingInfo);
    }

    void UploadAdd_TexturePackBinding(const Ref<Renderer>& rn, uint32_t bindingIndex, const Ref<RenderTexture>& rt, uint32_t attachmentIndex)
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

        UploadAdd_TexturePackBindings(rn, bindingInfo);
    }

    void UploadAdd_TexturePackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Texture>& info)
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

    void UploadAdd_TexturePackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<Cubemap>& info)
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

    void UploadAdd_TexturePackBindings(const Ref<Renderer>& rn, const TexturePackBindingUploadInfo<RenderTexture>& info)
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

            const auto packet = rn->currentDraw.packet;
            packet->dependencies.push_back(info);
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

            const auto packet = rn->currentDraw.packet;
            packet->renderTextures.push_back(RenderTexturePacketInfo
            {
                .texture = rt,
                .shaderLayoutRange = AssetRange{ .start = (uint16_t)packet->shaderLayouts.size(), .size = (uint16_t)0 },
                .dependencyRange = AssetRange{ .start = (uint8_t)packet->dependencies.size(), .size = (uint8_t)0 }
            });

            rn->currentDraw.currentRenderTexture = &packet->renderTextures[packet->renderTextures.size() - 1];
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

    void Start_ShaderLayout(const Ref<Renderer>& rn, ShaderLayout layout)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentRenderTexture)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot draw anything without starting render texture!");

            if (rn->currentDraw.currentShaderLayout)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start ShaderLayout without ending previous one!");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->shaderLayouts.push_back(ShaderLayoutPacketInfo
            {
                .layout = layout,
                .shaderPacketRange = AssetRange{ .start = (uint16_t)packet->shaders.size(), .size = (uint16_t)0 },
                .bufferSetRange = AssetRange{ .start = (uint32_t)packet->bufferSets.size(), .size = (uint32_t)0 },
            });

            rn->currentDraw.currentShaderLayout = &packet->shaderLayouts[packet->shaderLayouts.size() - 1];
            rn->currentDraw.currentRenderTexture->shaderLayoutRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to start shader setup!");
            throw;
        }
    }

    void End_ShaderLayout(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShaderLayout)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end ShaderLayout without starting it!");
#endif

        if (rn->currentDraw.currentShaderLayout->shaderPacketRange.size == 0)
        {
            rn->currentDraw.currentRenderTexture->shaderLayoutRange.size--;
            rn->currentDraw.packet->shaderLayouts.pop_back();
        }

        rn->currentDraw.currentShaderLayout = nullptr;
    }

    void Start_Shader(const Ref<Renderer>& rn, const Ref<Shader>& shader)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentShaderLayout)
                throw GENERIC_EXCEPT("[Hyperflow]", "ShaderLayout must be set!");

            if (rn->currentDraw.currentShader)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start Shader without ending previous one!");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->shaders.push_back(ShaderPacketInfo
            {
                .shader = shader,
                .materialPacketRange = AssetRange{ .start = (uint16_t)packet->materials.size(), .size = (uint16_t)0 }
            });

            rn->currentDraw.currentShader = &packet->shaders[packet->shaders.size() - 1];
            rn->currentDraw.currentShaderLayout->shaderPacketRange.size++;
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
        if (!rn->currentDraw.currentShaderLayout)
            throw GENERIC_EXCEPT("[Hyperflow]", "ShaderLayout must be set!");

        if (!rn->currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end Shader without starting it!");
#endif

        if (rn->currentDraw.currentShader->materialPacketRange.size == 0)
        {
            rn->currentDraw.currentShaderLayout->shaderPacketRange.size--;
            rn->currentDraw.packet->shaders.pop_back();
        }

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

            const auto packet = rn->currentDraw.packet;
            packet->materials.push_back(MaterialPacketInfo
            {
                .material = material,
                .drawGroupRange = AssetRange{ .start = (uint32_t)packet->drawPackets.size(), .size = (uint32_t)0 },
                .texpackRange = AssetRange{ .start = (uint32_t)packet->texpacks.size(), .size = (uint32_t)0 },
            });

            rn->currentDraw.currentMaterial = &packet->materials[packet->materials.size() - 1];
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

        if (rn->currentDraw.currentMaterial->drawGroupRange.size == 0)
        {
            rn->currentDraw.currentShader->materialPacketRange.size--;
            rn->currentDraw.packet->materials.pop_back();
        }

        rn->currentDraw.currentMaterial = nullptr;
    }

    void Start_DrawGroup(const Ref<Renderer>& rn)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentMaterial)
                throw GENERIC_EXCEPT("[Hyperflow]", "Material must be set!");

            if (rn->currentDraw.currentDrawPacket)
                throw GENERIC_EXCEPT("[Hyperflow]", "Need to end draw before starting new one!");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->drawPackets.push_back(DrawPacketInfo
            {
                .texpackRange = AssetRange{ .start = (uint32_t)packet->texpacks.size(), .size = (uint32_t)0 },
                .drawCallRange = AssetRange{ .start = (uint32_t)packet->drawCalls.size(), .size = (uint32_t)0 },
                .pushConstantRange = AssetRange{ .start = (uint32_t)packet->pushConstantUploads.size(), .size = (uint32_t)0 },
            });

            rn->currentDraw.currentDrawPacket = &packet->drawPackets[packet->drawPackets.size() - 1];
            rn->currentDraw.currentMaterial->drawGroupRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("Unable to start draw! Draw packet buffer usage: %llu", rn->currentDraw.packet->drawPackets.size());
            throw;
        }
    }

    void End_DrawGroup(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentShader)
            throw GENERIC_EXCEPT("[Hyperflow]", "Shader must be set!");

        if (!rn->currentDraw.currentDrawPacket)
            throw GENERIC_EXCEPT("[Hyperflow]", "Need to start draw before ending it!");
#endif

        if (rn->currentDraw.currentDrawPacket->drawCallRange.size == 0)
        {
            rn->currentDraw.currentMaterial->drawGroupRange.size--;
            rn->currentDraw.packet->drawPackets.pop_back();
        }

        rn->currentDraw.currentDrawPacket = nullptr;
    }

    void Start_BufferSet(const Ref<Renderer>& rn, RenderBindingType bindingType, uint32_t setBindingIndex)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentShaderLayout)
                throw GENERIC_EXCEPT("[Hyperflow]", "Shader Setup must be set until you set uniform set");
            if (rn->currentDraw.currentUniformSet)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start uniform set without ending previous one!");
#endif

            const auto packet = rn->currentDraw.packet;
            packet->bufferSets.push_back(BufferSetPacketInfo
            {
                .bindingType = bindingType,
                .setBindingIndex = setBindingIndex,
                .bufferRange = AssetRange{ .start = (uint16_t)packet->buffers.size(), .size = (uint16_t)0 },
            });
            rn->currentDraw.currentUniformSet = &packet->bufferSets[packet->bufferSets.size() - 1];
            rn->currentDraw.currentShaderLayout->bufferSetRange.size++;
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

            const auto packet = rn->currentDraw.packet;
            packet->buffers.push_back(buffer);
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

            const auto packet = rn->currentDraw.packet;
            TextureBindingInfo bindingInfo{};
            bindingInfo.pack = texPack;
            bindingInfo.setBindingIndex = setBindingIndex;

            packet->texpacks.push_back(bindingInfo);
            rn->currentDraw.currentMaterial->texpackRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to bind texture pack to material!");
            throw;
        }
    }

    void Start_DrawCall(const Ref<Renderer>& rn, const Ref<IndexBuffer>& indexBuffer)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentDrawPacket)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");

            if (rn->currentDraw.currentDrawCall)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start draw call without ending previous one!");
#endif

            const auto packet = rn->currentDraw.packet;
            DrawCallPacketInfo packetInfo{};
            packetInfo.vertexBufferRange = AssetRange{ .start = (uint32_t)packet->vertexBuffers.size(), .size = (uint32_t)0 };
            packetInfo.instanceRange = AssetRange{ .start = (uint32_t)packet->instances.size(), .size = (uint32_t)0 };
            packetInfo.indexBuffer = indexBuffer;
            packetInfo.instanceBufferOffset = (uint32_t)packet->instanceUploads.size();

            packet->drawCalls.push_back(packetInfo);
            rn->currentDraw.currentDrawPacket->drawCallRange.size++;
            rn->currentDraw.currentDrawCall = &packet->drawCalls[packet->drawCalls.size() - 1];
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to Start Draw Call!");
            throw;
        }
    }

    void Start_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh)
    {
        Start_DrawCall(rn, mesh->indexBuffer);
        DrawAdd_VertexBuffer(rn, mesh->vertBuffer);

#if DEBUG
        strcpy(rn->currentDraw.currentDrawCall->debugName, mesh->name.c_str());
#endif
    }

    void Start_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh, const Ref<Armature>& armature, uint32_t skinIndex)
    {
        Start_DrawCall(rn, mesh->indexBuffer);
        DrawAdd_VertexBuffer(rn, mesh->vertBuffer);

        if (skinIndex >= 0)
        {
            if (mesh->skinBuffers.size() <= skinIndex) throw GENERIC_EXCEPT("[Hyperflow]", "Skin index is out of bounds!");
            DrawAdd_VertexBuffer(rn, mesh->skinBuffers[skinIndex]);
        }

#if DEBUG
        strcpy(rn->currentDraw.currentDrawCall->debugName, mesh->name.c_str());
#endif
    }

    void End_DrawCall(const Ref<Renderer>& rn)
    {
#if DEBUG
        if (!rn->currentDraw.currentDrawCall)
            throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start draw call without ending previous one!");
#endif

        if (rn->currentDraw.currentDrawCall->isInstanced && rn->currentDraw.currentDrawCall->instanceRange.size == 0)
        {
            rn->currentDraw.currentDrawPacket->drawCallRange.size--;
            rn->currentDraw.packet->drawCalls.pop_back();
        }

        rn->currentDraw.currentDrawCall = nullptr;
    }

    void DrawGroupAdd_TexturePackBinding(const Ref<Renderer>& rn, const Ref<TexturePack>& texPack, uint32_t setBindingIndex)
    {
        try
        {
#if DEBUG
            if (!rn->currentDraw.currentDrawPacket)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");
#endif

            const auto packet = rn->currentDraw.packet;

            TextureBindingInfo bindingInfo{};
            bindingInfo.pack = texPack;
            bindingInfo.setBindingIndex = setBindingIndex;

            packet->texpacks.push_back(bindingInfo);
            rn->currentDraw.currentDrawPacket->texpackRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to bind texture pack to draw!");
            throw;
        }
    }

    void DrawGroupAdd_DrawCall(const Ref<Renderer>& rn, const Ref<IndexBuffer>& indexBuffer, const Ref<VertexBuffer>& vertexBuffer)
    {
        Start_DrawCall(rn, indexBuffer);
        DrawAdd_VertexBuffer(rn, vertexBuffer);

        rn->currentDraw.currentDrawCall->isInstanced = false;

        End_DrawCall(rn);
    }

    void DrawGroupAdd_DrawCall(const Ref<Renderer>& rn, const Ref<Mesh>& mesh)
    {
        Start_DrawCall(rn, mesh);
        rn->currentDraw.currentDrawCall->isInstanced = false;
        End_DrawCall(rn);
    }

    void DrawGroupSet_PushConstant(const Ref<Renderer>& rn, const void* data, uint32_t dataSize)
    {
        try
        {
            const auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentDrawPacket)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");

            if (currentDraw.currentDrawPacket->pushConstantRange.size > 0)
                throw GENERIC_EXCEPT("[Hyperflow]", "Cannot set push constant twice for a single draw packet!");
#endif

            const auto packet = currentDraw.packet;
            currentDraw.currentDrawPacket->pushConstantRange.size = dataSize;
            packet->pushConstantUploads.insert(packet->pushConstantUploads.end(),
                                            (uint8_t*)data,
                                            (uint8_t*)data + dataSize);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable to set push constant!");
            throw;
        }
    }

    void DrawAdd_Instance(const Ref<Renderer>& rn, const void* data, uint32_t dataSize, const VolumeTransform& volume)
    {
        try
        {
            const auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentDrawPacket)
                throw GENERIC_EXCEPT("[Hyperflow]", "DrawPacket must be set!");

            if (dataSize > MAX_PER_INSTANCE_BUFFER_SIZE)
                throw GENERIC_EXCEPT("[Hyperflow]", "Instance data cannot be more the %i bytes!", MAX_PER_INSTANCE_BUFFER_SIZE);

            if (currentDraw.packet->instanceUploads.size() + dataSize > MAX_INSTANCE_BUFFER_SIZE_BYTES)
                throw GENERIC_EXCEPT("[Hyperflow]", "Too much instance data, max size is %i bytes!", MAX_INSTANCE_BUFFER_SIZE_BYTES);

            if (currentDraw.currentDrawCall->singleInstanceSize != 0 && currentDraw.currentDrawCall->singleInstanceSize != dataSize)
                throw GENERIC_EXCEPT("[Hyperflow]", "Instance data size must be the same for all instances!");
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
            packet->instanceUploads.insert(packet->instanceUploads.end(),
                                            (uint8_t*)data,
                                            (uint8_t*)data + dataSize);
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable add instance!");
            throw;
        }
    }

    void DrawAdd_VertexBuffer(const Ref<Renderer>& rn, const Ref<VertexBuffer>& vb)
    {
        try
        {
            const auto& currentDraw = rn->currentDraw;
#if DEBUG
            if (!currentDraw.currentDrawCall)
                throw GENERIC_EXCEPT("[Hyperflow]", "You must be start draw call first!");
#endif

            const auto packet = currentDraw.packet;
            packet->vertexBuffers.push_back(vb);
            currentDraw.currentDrawCall->vertexBufferRange.size++;
        }
        catch (...)
        {
            LOG_ERROR("%s", "Unable add vertex buffer!");
            throw;
        }
    }

    namespace inter::rendering
    {
        void StartRenderPacket_i(const Ref<Renderer>& rn)
        {
#if DEBUG
            if (rn->currentDraw.packet) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot start RenderPacket, without submitting previous one!");
#endif

            std::lock_guard lock(rn->threadInfo.threadLock);
            rn->currentDraw.packet = rn->threadInfo.cachedPackets.back();
            rn->threadInfo.cachedPackets.pop_back();
            rn->currentDraw.packet->clear();
            rn->isDrawing = true;
        }

        void EndRenderPacket_i(const Ref<Renderer>& rn)
        {
#if DEBUG
            if (!rn->isDrawing) throw GENERIC_EXCEPT("[Hyperflow]", "Cannot end RenderPacket, without starting it!");
#endif

            {
                const auto packet = rn->currentDraw.packet;
                rn->lastDrawStatistics = packet->statistics;

                GlobalUniformInfo uniformInfo{};
                uniformInfo.camera = packet->camera.GetUniformInfo(rn);
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

                Upload_Buffer(rn, uniformUpload);
            }

            {
                std::lock_guard lock(rn->threadInfo.threadLock);;

                if (rn->threadInfo.drawPacket) rn->threadInfo.cachedPackets.push_back(rn->threadInfo.drawPacket);
                rn->threadInfo.drawPacket = rn->currentDraw.packet;
                rn->currentDraw.packet = nullptr;
                rn->threadInfo.renderCondition.notify_one();
            }

            rn->currentDraw.clear();
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

            uvec2 size = HF.renderingApi.api.GetReadyForRendering(rn->handle, renderTextures, packet->renderTextures.size());
            if (size != uvec2(0, 0))
            {
                for (const auto& rtPacket : packet->renderTextures) rtPacket.texture->createInfo.size = size;

                HF.renderingApi.api.StartFrame(rn->handle);
                RendererDraw_i(rn, packet);
                HF.renderingApi.api.EndFrame(rn->handle);
            }

            std::lock_guard lock(tInfo.threadLock);
            tInfo.cachedPackets.push_back(packet);
        }

        static void RendererDrawUploads(const void* handle, RenderPacket* packet);
        static void BindBuffers(const void* handle, const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);
        static void CollectTexpacks(const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings);

        void RendererDraw_i(const Ref<Renderer>& rn, RenderPacket* packet)
        {
            const auto handle = rn->handle;
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
                                        if (drawCall.vertexBufferRange.size > MAX_NUM_BUFFER_CACHE)
                                            throw GENERIC_EXCEPT("[Hyperflow]", "Trying to draw too many buffers at once, max is %i", MAX_NUM_BUFFER_CACHE);
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

                if (rt.drawCallback) rt.drawCallback(rn, HF.renderingApi.api.GetCmd(handle));
                HF.renderingApi.api.EndRendering(handle);
            }
        }

        void BindBuffers(const void* handle, const RenderPacket* packet,
            const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
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

        void CollectTexpacks(const RenderPacket* packet, const AssetRange<uint32_t>& range, DescriptorBindingInfo* descBindings)
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
