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

    struct TexPackBindingInfo
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
        AssetRange<uint32_t> instanceRange{};
        Ref<IndexBuffer> indexBuffer{};
        uint32_t instanceBufferOffset{};
        uint32_t singleInstanceSize{};
        bool isInstanced = true;

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
        AssetRange<uint32_t> drawGroupRange{};
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

        void (*drawCallback)(void* cmd){};
    };

    struct TextureInfo
    {
        void* texture{};
        uint32_t index{};
    };

    struct InstancePacketInfo
    {

    };

    struct RenderPacket
    {
        RendererStatistics statistics{};

        CameraFrustum frustum{};
        Camera3DFreeLook camera{};

        SmallList<DirectionalLight, RN_NUM_DIRECTIONAL_LIGHTS> directionalLights{};
        SmallList<SpotLight, RN_NUM_SPOT_LIGHTS> spotLights{};
        SmallList<PointLight, RN_NUM_POINT_LIGHTS> pointLights{};

        SmallList<RenderAttachmentDependencyInfo, RN_NUM_RENDER_ATTACHMENT_DEPENDENCIES> dependencies{};
        SmallList<RenderTexturePacketInfo, RN_NUM_RENDER_TEXTURES> renderTextures{};

        List<ShaderLayoutPacketInfo> shaderLayouts{};
        List<ShaderPacketInfo> shaders{};
        List<MaterialPacketInfo> materials{};
        List<DrawPacketInfo> drawPackets{};
        List<TexPackBindingInfo> texpacks{};
        List<TextureInfo> textures{};

        SmallList<TexturePackRebindingGroupPacketInfo, RN_NUM_TEX_PACK_GROUP_UPLOADS> textureGroupRebindings{};
        SmallList<TexturePackRebindingPacketInfo, RN_NUM_TEX_PACK_UPLOADS> textureRebindings{};
        SmallList<BufferUploadPacketInfo, RN_NUM_BUFFER_UPLOADS> bufferUploadPackets{};

        SmallList<BufferSetPacketInfo, RN_NUM_BUFFERS_SETS> bufferSets{};
        SmallList<Buffer, RN_NUM_BUFFERS> buffers{};

        List<DrawCallPacketInfo> drawCalls{};
        List<InstancePacketInfo> instances{};
        List<Ref<VertexBuffer>> vertexBuffers{};

        List<uint8_t> bufferUploads{};
        List<uint8_t> pushConstantUploads{};
        List<uint8_t> instanceUploads{};

        void preallocate()
        {
            bufferUploads.reserve(RN_NUM_BUFFER_UPLOAD_DATA);
            pushConstantUploads.reserve(RN_NUM_PUSH_CONSTANT_UPLOAD_DATA);
            instanceUploads.reserve(RN_NUM_INSTANCE_UPLOAD_DATA);
            shaderLayouts.reserve(RN_NUM_SHADER_LAYOUTS);
            shaders.reserve(RN_NUM_SHADERS);
            materials.reserve(RN_NUM_MATERIALS);
            drawPackets.reserve(RN_NUM_DRAW_PACKETS);
            texpacks.reserve(RN_NUM_TEX_PACKS);
            textures.reserve(RN_NUM_TEXTURE_UPLOADS);
            drawCalls.reserve(RN_NUM_DRAW_CALLS);
            instances.reserve(RN_NUM_DRAW_INSTANCES);
            vertexBuffers.reserve(RN_NUM_VERTEX_BUFFERS);
        }

        void clear()
        {
            statistics = RendererStatistics{};

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
            instances.clear();
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
            currentTexturePackBinding = nullptr;
            currentDrawCall = nullptr;
        }
    };
}

#endif //HPACKETS_H
