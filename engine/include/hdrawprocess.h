#ifndef HPACKETS_H
#define HPACKETS_H

#include "hshared.h"
#include "../config.h"
#include "../components/include/hstaticvector.h"

namespace hf
{
    struct TextureBindingInfo
    {
        Ref<TexturePack> pack{};
        uint32_t setBindingIndex{};
    };

    template<typename T>
    struct AssetRange
    {
        T start{}, size{};
        T end() const { return start + size; }
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
        Ref<TexturePack> texturePack;
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
        ShaderBindingInfo bindingInfo{};
        AssetRange<uint16_t> materialPacketRange{};
    };

    struct ShaderSetupPacketInfo
    {
        ShaderSetup shaderSetup{};
        AssetRange<uint16_t> shaderPacketRange{};
        AssetRange<uint32_t> uniformRange{};
    };

    struct RenderPassPacketInfo
    {
        RenderPass pass{};
        AssetRange<uint16_t> shaderSetupRange{};
    };

    struct UniformUploadPacketInfo
    {
        UniformBuffer buffer{};
        uint32_t offsetInBytes{};
        AssetRange<uint32_t> uniformRange{};
    };

    struct RenderPacket
    {
        StaticVector<RenderPassPacketInfo, RENDERING_MAX_NUM_RENDER_PASSES> passes{};
        StaticVector<ShaderSetupPacketInfo, RENDERING_MAX_NUM_SHADER_SETUPS> shaderSetups{};
        StaticVector<ShaderPacketInfo, RENDERING_MAX_NUM_SHADERS> shaders{};
        StaticVector<MaterialPacketInfo, RENDERING_MAX_NUM_MATERIALS> materials{};
        StaticVector<DrawPacketInfo, RENDERING_MAX_NUM_DRAWPACKETS> drawPackets{};
        StaticVector<TextureBindingInfo, RENDERING_MAX_NUM_TEXPACKS> texpacks{};

        StaticVector<TexturePackRebindingGroupPacketInfo, RENDERING_MAX_NUM_TEXPACK_REBINDING> textureGroupRebindings{};
        StaticVector<TexturePackRebindingPacketInfo, RENDERING_MAX_NUM_TEXPACK_REBINDING> textureRebindings{};
        StaticVector<void*, RENDERING_MAX_NUM_TEXTURES> textures{};

        StaticVector<UniformBufferBindInfo, RENDERING_MAX_NUM_UNIFORMS> uniforms{};
        StaticVector<DrawCallInfo, RENDERING_MAX_NUM_DRAW_CALLS> drawCalls{};
        StaticVector<uint8_t, RENDERING_MAX_UNIFORM_UPLOAD_BUFFER_SIZE> uniformUploads{};
        StaticVector<uint8_t, RENDERING_MAX_PUSH_CONSTANT_UPLOAD_BUFFER_SIZE> pushConstantUploads{};
        StaticVector<UniformUploadPacketInfo, RENDERING_MAX_UNIFORM_UPLOAD_COUNT> uniformUploadPackets{};
    };

    struct RenderPacketDrawProcess
    {
        RenderPassPacketInfo* currentPass{};
        ShaderSetupPacketInfo* currentShaderSetup{};
        ShaderPacketInfo* currentShader{};
        MaterialPacketInfo* currentMaterial{};
        DrawPacketInfo* currentDraw{};
        TexturePackRebindingGroupPacketInfo* currentTexturePackBinding{};

        RenderPacket packet{};
    };
}

#endif //HPACKETS_H
