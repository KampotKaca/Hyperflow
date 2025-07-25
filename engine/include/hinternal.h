#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
#include "hvertbuffer.h"
#include "hindexbuffer.h"
#include "hcubemap.h"
#include "htime.h"
#include "../rendering/include/hex_renderer.h"
#include "../platforms/include/hex_platform.h"
#include "audio/haudioclip.h"
#include "audio/haudioplayer.h"

#if PLATFORM_LINUX
#ifdef None
#undef None
#endif
#endif

namespace hf::inter
{
    struct RenderingApi
    {
        RenderingApiType type = RenderingApiType::None;
        void* handle{};
        bool isLoaded = false;
        rendering::RendererAPI api{};
    };

    struct GraphicsResources
    {
        unordered_map<uint64_t, Ref<Shader>> shaders{};
        unordered_map<uint64_t, Ref<RuntimeBufferBase>> buffers{};
        unordered_map<uint64_t, Ref<RenderTexture>> renderTextures{};
        unordered_map<uint64_t, Ref<TexturePack>> texturePacks{};
        unordered_map<uint64_t, Ref<TexturePackAllocator>> texturePackAllocators{};

        unordered_map<std::string, BufferAttrib> bufferAttribs{};
        unordered_map<std::string, Ref<Mesh>> meshes{};
        unordered_map<std::string, Ref<Texture>> textures{};
        unordered_map<std::string, Ref<Cubemap>> cubemaps{};

        unordered_map<uint64_t, Ref<Material>> materials{};

        Buffer materialDataStorageBuffer{};
    };

    struct AudioResources
    {
        unordered_map<std::string, Ref<AudioClip>> clips{};
        unordered_map<uint64_t, Ref<AudioPlayer>> players{};
        unordered_map<uint64_t, Ref<AudioPlayer3D>> player3Ds{};
    };

    struct ResourcesMarkedForDeletion
    {
        struct TypedBuffer
        {
            void* buffer{};
            RuntimeBufferType type{};
        };

        std::mutex syncLock{};
        std::vector<void*> shaders{};
        std::vector<TypedBuffer> buffers{};
        std::vector<void*> texturePacks{};
        std::vector<void*> texturePackAllocators{};
        std::vector<void*> textures{};
        std::vector<void*> renderTextures{};
    };

    struct StaticResources
    {
        BufferAttrib quadAttrib{};
        BufferAttrib defaultAttrib{};

        TextureLayout skyboxLayout{};
        TextureSampler cubemapSampler{};

        Buffer globalUniform = 0;
        BufferAllocator bufferAllocator = 0;

        ShaderLayout axisLinesShaderLayout{};
        ShaderLayout skyboxShaderLayout{};

        Ref<Shader> axisLinesShader{};
        Ref<Shader> skyboxShader{};

        Ref<Cubemap> defaultSkyboxCubemap{};
        Ref<Cubemap> boundCubemap{};

        Ref<TexturePack> skyboxTexturePack{};

        Ref<TexturePackAllocator> texPackAllocator{};

        Ref<Mesh> cube{};
        Ref<Mesh> plane{};
        Ref<Mesh> sphere{};

        Ref<Material> emptyMaterial{};

        Ref<VertBuffer> quadBuffer{};
    };

    struct Hyperflow
    {
        EngineLifecycleCallbacks lifecycleCallbacks{};
        EngineUpdateType updateType = EngineUpdateType::Continues;
        EngineInternalResourceFormatInfo internalResourcesFormat{};
        EngineInternalAudioInfo internalAudioInfo{};
        std::atomic_bool isRunning{};
        std::string appTitle{};
        Time time{};
        Ref<Window> mainWindow{};
        std::vector<Ref<Window>> tempWindows{};
        unordered_map<uint64_t, Ref<Window>> windows{};
        uint32_t rendererCount{};
        RenderingApi renderingApi{};

        GraphicsResources graphicsResources{};
        ResourcesMarkedForDeletion deletedResources{};
        StaticResources staticResources{};
        AudioResources audioResources{};
    };

    extern Hyperflow HF;

    //if true, will release internal resources, but will retain resource references
    //it is made to be used for handling api changes.
    void UnloadAllResources_i(bool internalOnly = false);
    void CleanMarkedResources_i();

    namespace alloc
    {
        void LoadAllocatorThread_i();
        void UnloadAllocatorThread_i();
        void UnloadAllocator_i();

        void LogMemoryStats_i();
        void LogThreadMemoryStats_i();
    }

    namespace primitives
    {
        void DefineStaticResources_i();
        void LoadStaticResources_i();
    }

    namespace audio
    {
        void Load_i();
        void Unload_i();

        bool CreateClip_i(AudioClip* clip);
        bool DestroyClip_i(AudioClip* clip);
    }

    namespace rendering
    {
        void StartRenderPacket_i(const Ref<Renderer>& rn);
        void EndRenderPacket_i(const Ref<Renderer>& rn);
        void PreDraw_i(const Ref<Renderer>& rn);

        void LoadApi_i(RenderingApiType api);
        void UnloadCurrentApi_i(bool retainReferences);

        bool CreateRenderTexture_i(RenderTexture* tex);
        bool DestroyRenderTexture_i(RenderTexture* tex);

        void CreateRenderer_i(Renderer* rn);
        void DestroyRenderer_i(Renderer* rn);
        void RunRenderThread_i(const Ref<Renderer>& rn);

        bool SetWindowIcons_i(const Window* win, const char* folderPath);

        void RendererUpdate_i(const Ref<Renderer>& rn);
        void RendererDraw_i(const Ref<Renderer>& rn, RenderPacket& packet);

        bool CreateShader_i(Shader* shader);
        bool DestroyShader_i(Shader* shader);

        bool CreateVertBuffer_i(VertBuffer* buffer);
        bool CreateIndexBuffer_i(IndexBuffer* buffer);

        bool DestroyBuffer_i(RuntimeBufferBase* buffer);

        bool CreateMesh_i(Mesh* mesh);
        bool DestroyMesh_i(Mesh* mesh);

        bool CreateTexture_i(Texture* tex);
        bool DestroyTexture_i(Texture* tex);

        bool CreateCubemap_i(Cubemap* cubemap);
        bool DestroyCubemap_i(Cubemap* cubemap);

        bool CreateTexturePack_i(TexturePack* texPack);
        bool DestroyTexturePack_i(TexturePack* texPack);

        bool CreateTexturePackAllocator_i(TexturePackAllocator* packAllocator);
        bool DestroyTexturePackAllocator_i(TexturePackAllocator* packAllocator);

        void DestroyAllBuffers_i(bool internalOnly = false);
        void DestroyAllMeshes_i(bool internalOnly = false);
        void DestroyAllTextures_i(bool internalOnly = false);
        void DestroyAllCubemaps_i(bool internalOnly = false);
        void DestroyAllTexturePacks_i(bool internalOnly = false);
        void DestroyAllRenderTextures_i(bool internalOnly = false);
        void DestroyAllTexturePackAllocators_i(bool internalOnly = false);
        void DestroyAllShaders_i(bool internalOnly = false);
    }
}

#endif //HINTERNAL_H
