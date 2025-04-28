#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
#include "hvertbuffer.h"
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
        bool isLoaded = false;
        rendering::RendererAPI api{};
    };

    struct GraphicsResources
    {
        std::unordered_map<Shader*, Ref<Shader>> shaders{};
        std::unordered_map<VertBuffer*, Ref<VertBuffer>> vertBuffers{};
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
        void LoadAllocatorThread();
        void UnloadAllocatorThread();
        void UnloadAllocator();

        void LogMemoryStats();
        void LogThreadMemoryStats();
    }

    namespace rendering
    {
        void LoadApi(RenderingApiType api);
        void UnloadCurrentApi(bool retainReferences);

        void CreateRenderer(Renderer* rn);
        void DestroyRenderer(Renderer* rn);

        bool DestroyShader_i(Shader* shader);
        bool DestroyVertBuffer_i(VertBuffer* buffer);
    }
}

#endif //HINTERNAL_H
