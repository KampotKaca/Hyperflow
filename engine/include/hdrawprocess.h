#ifndef HPACKETS_H
#define HPACKETS_H

#include "../components/include/hcomponents.h"
#include "hshared.h"
#include "../config.h"

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

    struct DrawCallPacketInfo
    {
        AssetRange<uint32_t> vertexBufferRange{};
        AssetRange<uint32_t> instanceDataRange{};
        Ref<IndexBuffer> indexBuffer{};
        uint32_t instanceCount{};

#if DEBUG
        char debugName[16];
#endif
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
        std::optional<Camera3DFreeLook> camera{};
        std::vector<DirectionalLight> directionalLights{};
        std::vector<SpotLight> spotLights{};
        std::vector<PointLight> pointLights{};

        std::vector<RenderAttachmentDependencyInfo> dependencies{};
        std::vector<RenderTexturePacketInfo> renderTextures{};
        std::vector<ShaderLayoutPacketInfo> shaderLayouts{};
        std::vector<ShaderPacketInfo> shaders{};
        std::vector<MaterialPacketInfo> materials{};
        std::vector<DrawPacketInfo> drawPackets{};
        std::vector<TextureBindingInfo> texpacks{};

        std::vector<TexturePackRebindingGroupPacketInfo> textureGroupRebindings{};
        std::vector<TexturePackRebindingPacketInfo> textureRebindings{};
        std::vector<TextureInfo> textures{};

        std::vector<BufferSetPacketInfo> bufferSets{};
        std::vector<Buffer> buffers{};

        std::vector<DrawCallPacketInfo> drawCalls{};
        std::vector<Ref<VertexBuffer>> vertexBuffers{};
        std::vector<uint8_t> bufferUploads{};
        std::vector<uint8_t> pushConstantUploads{};
        std::vector<uint8_t> instanceUploads{};
        std::vector<BufferUploadPacketInfo> bufferUploadPackets{};

        void clear()
        {
            camera = std::nullopt;
            directionalLights.clear();
            spotLights.clear();
            pointLights.clear();

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
            vertexBuffers.clear();
            bufferUploads.clear();
            pushConstantUploads.clear();
            instanceUploads.clear();
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
        DrawPacketInfo* currentDrawPacket{};
        TexturePackRebindingGroupPacketInfo* currentTexturePackBinding{};
        DrawCallPacketInfo* currentDrawCall{};

        RenderPacket* packet{};

        void clear()
        {
            currentRenderTexture = nullptr;
            currentShaderLayout = nullptr;
            currentShader = nullptr;
            currentMaterial = nullptr;
            currentUniformSet = nullptr;
            currentDrawPacket = nullptr;
            currentDrawCall = nullptr;
            currentTexturePackBinding = nullptr;
        }
    };
}

#endif //HPACKETS_H
