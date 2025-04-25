#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
#include "hvertbuffer.h"
#include "components/htime.h"

namespace hf::inter
{
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
        RenderingApi renderingApi = RenderingApi::None;

        std::unordered_map<Shader*, Ref<Shader>> shaders{};
        std::unordered_map<VertBuffer*, Ref<VertBuffer>> vertBuffers{};
    };

    extern Hyperflow HF;

    void* LoadDll(const char* dllName);
    void UnloadDll(void* dll);

    namespace window
    {
        void Open(Window* win);
        bool Close(Window* win);

        void SetTitle(const Window* win, const std::string& title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);

        void SetFlags(Window* win, WindowFlags flags);
        void Focus(const Window* win);
        void Update(const Window* win);
    }

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
        void LoadApi(RenderingApi api);
        void UnloadCurrentApi(bool retainReferences);

        void CreateRenderer(Renderer* rn);
        void DestroyRenderer(Renderer* rn);

        bool DestroyShader_i(Shader* shader);
        bool DestroyVertBuffer_i(VertBuffer* buffer);
    }
}

#endif //HINTERNAL_H
