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
        void* additionalDll{};
        bool isLoaded = false;
        rendering::RendererAPI api{};
    };

    struct GraphicsResources
    {
        phmap::flat_hash_map<uint64_t, Ref<Shader>> shaders{};
        phmap::flat_hash_map<uint64_t, Ref<VertBuffer>> vertBuffers{};
        phmap::flat_hash_map<uint64_t, Ref<IndexBuffer>> indexBuffers{};
        phmap::flat_hash_map<uint64_t, Ref<StorageBuffer>> storageBuffers{};
        phmap::flat_hash_map<uint64_t, Ref<TexturePack>> texturePacks{};
        phmap::flat_hash_map<uint64_t, Ref<TexturePackAllocator>> texturePackAllocators{};

        phmap::flat_hash_map<std::string, BufferAttrib> bufferAttribs{};
        phmap::flat_hash_map<std::string, Ref<Mesh>> meshes{};
        phmap::flat_hash_map<std::string, Ref<Texture>> textures{};
        phmap::flat_hash_map<std::string, Ref<Cubemap>> cubemaps{};

        Ref<StorageBuffer> materialDataStorage{};
    };

    struct ResourcesMarkedForDeletion
    {
        std::mutex syncLock{};
        std::vector<void*> shaders{};
        std::vector<void*> vertBuffers{};
        std::vector<void*> indexBuffers{};
        std::vector<void*> storageBuffers{};
        std::vector<void*> texturePacks{};
        std::vector<void*> texturePackAllocators{};
        std::vector<void*> textures{};
    };

    struct StaticResources
    {
        TextureLayout emptyLayout{};
        BufferAttrib quadAttrib{};
        Ref<VertBuffer> quadBuffer{};

        TextureSampler cubemapSampler{};
        BufferAttrib cubeAttrib{};

        Ref<VertBuffer> cubeVertices{};
        Ref<IndexBuffer> cubeIndices{};
    };

    struct Hyperflow
    {
        EngineLifecycleCallbacks lifecycleCallbacks{};
        EngineUpdateType updateType = EngineUpdateType::Continues;
        std::atomic_bool isRunning{};
        std::string appTitle{};
        Time time{};
        Ref<Window> mainWindow{};
        phmap::flat_hash_map<uint64_t, Ref<Window>> windows{};
        uint32_t rendererCount{};
        RenderingApi renderingApi{};

        GraphicsResources graphicsResources{};
        ResourcesMarkedForDeletion deletedResources{};
        StaticResources staticResources{};
    };

    extern Hyperflow HF;

    struct AxisLinePushConstant
    {
        vec4 planeNormal; // w is thickness
        vec4 color;
    };

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

    namespace rendering
    {
        void StartRenderPacket_i(const Ref<Renderer>& rn);
        void EndRenderPacket_i(const Ref<Renderer>& rn);

        void LoadApi_i(RenderingApiType api);
        void UnloadCurrentApi_i(bool retainReferences);
        void DefineStaticResources_i();
        void LoadStaticResources_i();

        void CreateRenderer_i(Renderer* rn);
        void DestroyRenderer_i(Renderer* rn);
        void RunRenderThread_i(const Ref<Renderer>& rn);

        bool SetWindowIcons_i(const Window* win, const char* folderPath);

        void RendererUpdate_i(const Ref<Renderer>& rn);
        void RendererDraw_i(const Ref<Renderer>& rn, const RenderPacket& packet);

        bool CreateShader_i(Shader* shader);
        bool DestroyShader_i(Shader* shader);

        bool CreateVertBuffer_i(VertBuffer* buffer);
        bool DestroyVertBuffer_i(VertBuffer* buffer);

        bool CreateIndexBuffer_i(IndexBuffer* buffer);
        bool DestroyIndexBuffer_i(IndexBuffer* buffer);

        bool CreateStorageBuffer_i(StorageBuffer* buffer);
        bool DestroyStorageBuffer_i(StorageBuffer* buffer);

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

        void DestroyAllVertBuffers_i(bool internalOnly = false);
        void DestroyAllIndexBuffers_i(bool internalOnly = false);
        void DestroyAllStorageBuffers_i(bool internalOnly = false);
        void DestroyAllMeshes_i(bool internalOnly = false);
        void DestroyAllTextures_i(bool internalOnly = false);
        void DestroyAllCubemaps_i(bool internalOnly = false);
        void DestroyAllTexturePacks_i(bool internalOnly = false);
        void DestroyAllTexturePackAllocators_i(bool internalOnly = false);
        void DestroyAllShaders_i(bool internalOnly = false);

        void SendMaterialPushConstants_i(Material* material);
    }
}

#endif //HINTERNAL_H
