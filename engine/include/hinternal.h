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
        std::unordered_map<Shader*, Ref<Shader>> shaders{};
        std::unordered_map<VertBuffer*, Ref<VertBuffer>> vertBuffers{};
        std::unordered_map<IndexBuffer*, Ref<IndexBuffer>> indexBuffers{};
        std::unordered_map<StorageBuffer*, Ref<StorageBuffer>> storageBuffers{};
        std::unordered_map<Mesh*, Ref<Mesh>> meshes{};
        std::unordered_map<Texture*, Ref<Texture>> textures{};
        std::unordered_map<TexturePack*, Ref<TexturePack>> texturePacks{};
        std::unordered_map<TexturePackAllocator*, Ref<TexturePackAllocator>> texturePackAllocators{};
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
