#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
#include "hvertexbuffer.h"
#include "hindexbuffer.h"
#include "hcubemap.h"
#include "hasset.h"
#include "hshaderlibrary.h"
#include "htime.h"
#include "hintertypes.h"
#include "../rendering/include/hex_renderer.h"
#include "../platforms/include/hex_platform.h"
#include "audio/haudioclip.h"
#include "audio/haudioplayer.h"

#if PLATFORM_LINUX
#ifdef None
#undef None
#endif
#endif

namespace hf::ir
{
    struct GlobalUniformInfo
    {
        CameraUniformInfo camera{};
        TimeUniformInfo time{};
        LightUniformInfo light{};
    };

    struct RenderingApi
    {
        RenderingApiType type = RenderingApiType::None;
        void* handle{};
        bool isLoaded = false;
        rdr::RendererAPI api{};
    };

    struct GraphicsResources
    {
        Dictionary<uint64_t, Ref<ShaderLibrary>> shaderLibraries{};
        Dictionary<uint64_t, Ref<Shader>> shaders{};
        Dictionary<uint64_t, Ref<RuntimeBufferBase>> buffers{};
        Dictionary<uint64_t, Ref<Texture>> textures{};
        Dictionary<uint64_t, Ref<Cubemap>> cubemaps{};
        Dictionary<uint64_t, Ref<RenderTexture>> renderTextures{};
        Dictionary<uint64_t, Ref<TexturePack>> texturePacks{};
        Dictionary<uint64_t, Ref<Mesh>> meshes{};

        Dictionary<std::string, ShaderLayout> shaderLayouts{};
        Dictionary<std::string, Buffer> bufferIds{};
        Dictionary<std::string, VertexBufferAttribute> vertexAttributes{};
        Dictionary<std::string, TextureLayout> textureLayouts{};
        Dictionary<std::string, TextureSampler> textureSamplers{};

        Dictionary<std::string, Asset> assets{};

        Dictionary<uint64_t, Ref<Material>> materials{};

        Buffer materialDataStorageBuffer{};
    };

    struct AudioResources
    {
        Dictionary<uint64_t, Ref<AudioPlayer>> players{};
        Dictionary<uint64_t, Ref<AudioPlayer3D>> player3Ds{};
        Dictionary<uint64_t, Ref<AudioGroup>> groups{};
    };

    struct GeneralResources
    {
        Dictionary<uint64_t, Ref<AnimationClip>> animations{};
        Dictionary<uint64_t, Ref<Armature>> armatures{};
    };

    struct ResourcesMarkedForDeletion
    {
        struct TypedBuffer
        {
            void* buffer{};
            RuntimeBufferType type{};
        };

        std::mutex syncLock{};
        SmallList<void*, EN_NUM_RESOURCES_MARKED_FOR_DELETION> shaderLibraries{};
        SmallList<void*, EN_NUM_RESOURCES_MARKED_FOR_DELETION> shaders{};
        SmallList<TypedBuffer, EN_NUM_RESOURCES_MARKED_FOR_DELETION> buffers{};
        SmallList<void*, EN_NUM_RESOURCES_MARKED_FOR_DELETION> texturePacks{};
        SmallList<void*, EN_NUM_RESOURCES_MARKED_FOR_DELETION> textures{};
        SmallList<void*, EN_NUM_RESOURCES_MARKED_FOR_DELETION> renderTextures{};
    };

    struct StaticVertexAttributes
    {
        VertexBufferAttribute quad{};
        VertexBufferAttribute pos_nor_tex{};
    };

    struct StaticResourcesLibraryModules
    {
        uint32_t quadVertexInput{};
        uint32_t defaultVertexInput{};

        uint32_t axisLinesPreRaster{};
        uint32_t skyboxPreRaster{};

        uint32_t axisLinesFragment{};
        uint32_t skyboxFragment{};

        uint32_t axisLinesFragmentOutput{};
        uint32_t skyboxFragmentOutput{};
    };

    struct StaticShaderLayouts
    {
        ShaderLayout axisLines{};
        ShaderLayout skybox{};
    };

    struct StaticShaders
    {
        Ref<Shader> axisLines{};
        Ref<Shader> skybox{};
    };

    struct StaticSkyboxResources
    {
        Ref<Cubemap> defaultCubemap{};
        Ref<Cubemap> boundCubemap{};

        Ref<TexturePack> texturePack{};
    };

    struct StaticResources
    {
        Buffer globalUniform{};

        TextureLayout skyboxLayout{};
        TextureSampler cubemapSampler{};

        Ref<ShaderLibrary> engineShadersLib{};

        StaticShaderLayouts shaderLayouts{};
        StaticShaders shaders{};

        StaticVertexAttributes vertexAttributes{};
        StaticResourcesLibraryModules engineShadersLibModules{};
        Ref<Model> primitiveModels[(uint32_t)PrimitiveMeshType::Count]{};
        Ref<Texture> primitiveTextures[(uint32_t)PrimitiveTextureType::Count]{};
        StaticSkyboxResources skyboxResources{};

        Ref<VertexBuffer> quadBuffer{};
        Ref<VertexBuffer> instanceBuffer{};
    };

    struct Hyperflow
    {
        EngineLifecycleCallbacks lifecycleCallbacks{};
        EngineInternalResourceFormatInfo internalResourcesFormat{};
        EngineInternalAudioInfo internalAudioInfo{};
        std::atomic_bool isRunning{};
        std::string appTitle{};
        Time time{};
        URef<Window> window{};
		URef<Renderer> renderer{};

        Dictionary<uint64_t, Ref<Scene>> scenes{};
        uint32_t rendererCount{};
        RenderingApi renderingApi{};

        GraphicsResources graphicsResources{};
        GeneralResources generalResources{};
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

    namespace audio
    {
        void Load_i();
        void Unload_i();

        Ref<AudioClip> CreateAudioClipAsset_i(const char* assetPath);
        bool DestroyAudioClip_i(AudioClip* clip);
    }

    namespace general
    {
        void LoadScene_i(Scene* scene);
        void UnloadScene_i(Scene* scene);

        void DefineStaticResources_i();
        void LoadStaticResources_i();

        void DestroyAnimationClip_i(AnimationClip* anim);
    }

    namespace rdr
    {
        void StartRenderPacket_i();
        void EndRenderPacket_i();
        void PreDraw_i();

        void LoadApi_i(RenderingApiType api);
        void UnloadCurrentApi_i(bool retainReferences);

        bool CreateRenderTexture_i(RenderTexture* tex);
        bool DestroyRenderTexture_i(RenderTexture* tex);

        void CreateRenderer_i(Renderer* rn);
        void DestroyRenderer_i(Renderer* rn);
        void RunRenderThread_i();
        void ResizeRenderer_i(uvec2 size);

        bool SetWindowIcons_i(const Window* win, const char* folderPath);

        void RendererUpdate_i();
        void RendererDraw_i(RenderPacket* packet);

        Ref<ShaderLibrary> CreateShaderLibraryAsset_i(const char* assetPath);
        bool DestroyShaderLibrary_i(ShaderLibrary* lib);

        Ref<Shader> CreateShaderAsset_i(const char* assetPath);
        bool DestroyShader_i(Shader* shader);

        bool CreateVertBuffer_i(VertexBuffer* buffer);
        bool CreateIndexBuffer_i(IndexBuffer* buffer);

        bool DestroyBuffer_i(RuntimeBufferBase* buffer);

        Ref<Model> CreateModelAsset_i(const char* assetPath);
        bool DestroyModel_i(Model* model);
        bool DestroyMesh_i(Mesh* mesh);

        Ref<Texture> CreateTextureAsset_i(const char* assetPath);
        bool DestroyTexture_i(Texture* tex);

        Ref<Cubemap> CreateCubemapAsset_i(const char* assetPath);
        bool DestroyCubemap_i(Cubemap* cubemap);

        Ref<TexturePack> CreateTexPackAsset_i(const char* assetPath);
        bool DestroyTexturePack_i(TexturePack* texPack);

        void DestroyAllBuffers_i(bool internalOnly = false);
        void DestroyAllAssets_i(bool internalOnly = false);
        void DestroyAllTexturePacks_i(bool internalOnly = false);
        void DestroyAllRenderTextures_i(bool internalOnly = false);
        void DestroyAllShaders_i(bool internalOnly = false);
        void DestroyAllShaderLibraries_i(bool internalOnly = false);
        void DestroyAllMeshes_i(bool internalOnly = false);
        void DestroyAllTextures_i(bool internalOnly = false);
        void DestroyAllCubemaps_i(bool internalOnly = false);

    }
}

#endif //HINTERNAL_H
