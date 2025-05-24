#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
#include "hvertbuffer.h"
#include "hindexbuffer.h"
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
    };

    struct Hyperflow
    {
        EngineLifecycleCallbacks lifecycleCallbacks{};
        EngineUpdateType updateType = EngineUpdateType::Continues;
        bool isRunning{};
        std::string appTitle{};
        Time time{};
        Ref<Window> mainWindow{};
        std::vector<Ref<Window>> windows{};
        uint32_t rendererCount{};
        RenderingApi renderingApi{};

        GraphicsResources graphicsResources{};
    };

    extern Hyperflow HF;

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
        void LoadApi_i(RenderingApiType api);
        void UnloadCurrentApi_i(bool retainReferences);

        void CreateRenderer_i(Renderer* rn);
        void DestroyRenderer_i(Renderer* rn);

        void UpdateRenderer_i(const Ref<Renderer>& rn);

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

        bool CreateTexturePack_i(TexturePack* texPack);
        bool DestroyTexturePack_i(TexturePack* texPack);

        bool CreateTexturePackAllocator_i(TexturePackAllocator* packAllocator);
        bool DestroyTexturePackAllocator_i(TexturePackAllocator* packAllocator);
    }
}

#endif //HINTERNAL_H
