#ifndef HPACKETS_H
#define HPACKETS_H

#include "hshared.h"
#include "../config.h"
#include "../components/include/hstaticvector.h"

namespace hf
{
    enum DescriptorType { BUFFER, TEXPACK };
    struct DescriptorBindingInfo
    {
        void* object{};
        DescriptorType type{};
    };

    struct TextureBindingInfo
    {
        Ref<TexturePack> pack{};
        uint32_t setBindingIndex{};
    };

    template<typename T>
    struct AssetRange
    {
        T start{}, size{};
        [[nodiscard]] T end() const { return start + size; }
    };

    struct BufferSetPacketInfo
    {
        RenderBindingType bindingType = RenderBindingType::Graphics;
        uint32_t setBindingIndex = 0;
        AssetRange<uint16_t> bufferRange{};
    };

    struct TexturePackRebindingPacketInfo
    {
        struct TextureBinding
        {
            uint32_t offset{};
            TexturePackBindingType type{};
            AssetRange<uint16_t> range{};
        };

        uint32_t bindingIndex = 0;
        std::optional<TextureSampler> sampler{};
        std::optional<TextureBinding> textures{};
    };

    struct TexturePackRebindingGroupPacketInfo
    {
        Ref<TexturePack> texturePack{};
        AssetRange<uint16_t> bindingPacketRange{};
    };

    struct DrawPacketInfo
    {
        AssetRange<uint32_t> texpackRange{};
        AssetRange<uint32_t> drawCallRange{};
        AssetRange<uint32_t> pushConstantRange{};
    };

    struct MaterialPacketInfo
    {
        Ref<Material> material{};
        AssetRange<uint32_t> drawPacketRange{};
        AssetRange<uint32_t> texpackRange{};
    };

    struct ShaderPacketInfo
    {
        Ref<Shader> shader{};
        AssetRange<uint16_t> materialPacketRange{};
    };

    struct ShaderLayoutPacketInfo
    {
        ShaderLayout layout{};
        AssetRange<uint16_t> shaderPacketRange{};
        AssetRange<uint32_t> bufferSetRange{};
    };

    struct BufferUploadPacketInfo
    {
        Buffer buffer{};
        uint32_t offsetInBytes{};
        AssetRange<uint32_t> bufferRange{};
    };

    struct RenderTexturePacketInfo
    {
        Ref<RenderTexture> texture{};
        AssetRange<uint16_t> shaderLayoutRange{};
        AssetRange<uint8_t> dependencyRange{};

        void (*drawCallback)(const Ref<Renderer>& rn, void* cmd){};
    };

    struct TextureInfo
    {
        void* texture{};
        uint32_t index{};
    };

    struct RenderPacket
    {
        StaticVector<RenderAttachmentDependencyInfo, RENDERING_MAX_NUM_RENDER_ATTACHMENT_DEPENDENCIES> dependencies{};
        StaticVector<RenderTexturePacketInfo, RENDERING_MAX_NUM_RENDER_TEXTURES> renderTextures{};
        StaticVector<ShaderLayoutPacketInfo, RENDERING_MAX_NUM_SHADER_LAYOUTS> shaderLayouts{};
        StaticVector<ShaderPacketInfo, RENDERING_MAX_NUM_SHADERS> shaders{};
        StaticVector<MaterialPacketInfo, RENDERING_MAX_NUM_MATERIALS> materials{};
        StaticVector<DrawPacketInfo, RENDERING_MAX_NUM_DRAWPACKETS> drawPackets{};
        StaticVector<TextureBindingInfo, RENDERING_MAX_NUM_TEXPACKS> texpacks{};

        StaticVector<TexturePackRebindingGroupPacketInfo, RENDERING_MAX_NUM_TEXPACK_REBINDING> textureGroupRebindings{};
        StaticVector<TexturePackRebindingPacketInfo, RENDERING_MAX_NUM_TEXPACK_REBINDING> textureRebindings{};
        StaticVector<TextureInfo, RENDERING_MAX_NUM_TEXTURES> textures{};

        StaticVector<BufferSetPacketInfo, RENDERING_MAX_NUM_UNIFORMS> bufferSets{};
        StaticVector<Buffer, RENDERING_MAX_NUM_UNIFORMS> buffers{};

        StaticVector<DrawCallInfo, RENDERING_MAX_NUM_DRAW_CALLS> drawCalls{};
        StaticVector<uint8_t, RENDERING_MAX_UNIFORM_UPLOAD_BUFFER_SIZE> bufferUploads{};
        StaticVector<uint8_t, RENDERING_MAX_PUSH_CONSTANT_UPLOAD_BUFFER_SIZE> pushConstantUploads{};
        StaticVector<BufferUploadPacketInfo, RENDERING_MAX_UNIFORM_UPLOAD_COUNT> bufferUploadPackets{};

        void clear()
        {
            dependencies.clear();
            renderTextures.clear();
            shaderLayouts.clear();
            shaders.clear();
            materials.clear();
            drawPackets.clear();
            texpacks.clear();

            textureGroupRebindings.clear();
            textureRebindings.clear();
            textures.clear();

            bufferSets.clear();
            buffers.clear();

            drawCalls.clear();
            bufferUploads.clear();
            pushConstantUploads.clear();
            bufferUploadPackets.clear();
        }
    };

    struct RenderPacketDrawProcess
    {
        RenderTexturePacketInfo* currentRenderTexture{};
        ShaderLayoutPacketInfo* currentShaderLayout{};
        ShaderPacketInfo* currentShader{};
        MaterialPacketInfo* currentMaterial{};
        BufferSetPacketInfo* currentUniformSet{};
        DrawPacketInfo* currentDraw{};
        TexturePackRebindingGroupPacketInfo* currentTexturePackBinding{};

        RenderPacket* packet{};

        void clear()
        {
            currentRenderTexture = nullptr;
            currentShaderLayout = nullptr;
            currentShader = nullptr;
            currentMaterial = nullptr;
            currentUniformSet = nullptr;
            currentDraw = nullptr;
            currentTexturePackBinding = nullptr;
        }
    };
}

#endif //HPACKETS_H
