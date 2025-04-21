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

        std::unordered_map<Shader*, Ref<Shader>> shaders{};
        std::unordered_map<VertBuffer*, Ref<VertBuffer>> vertBuffers{};
    };

    extern Hyperflow HF;

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
        struct ShaderCreationInfo
        {
            uint32_t supportedAttribCount{};
            const BufferAttrib* pSupportedAttribs{};

            const char* vCode;
            uint32_t vCodeSize;
            const char* fCode;
            uint32_t fCodeSize;
        };

        void Load(const char* version);
        void Unload();

        void* CreateInstance(void* handle, uvec2 size);
        void DestroyInstance(void* rnInstance);

        void* CreateShader(const ShaderCreationInfo& info);
        void DestroyShader(void* shader);
        bool DestroyShader_i(Shader* shader);

        uint32_t CreateBufferAttrib(const BufferAttribCreateInfo& info, uint32_t fullStride);

        void* CreateVertBuffer(const VertBufferCreationInfo& info);
        void DestroyVertBuffer(void* handle);
        bool DestroyVertBuffer_i(VertBuffer* buffer);

        void BindShader(const void* renderer, const void* shader, BufferAttrib attrib);

        bool StartFrame(Renderer* rn);
        void EndFrame(Renderer* rn);
        void RegisterFrameBufferChange(Renderer* rn, uvec2 newSize);

        void Draw(Renderer* rn);
        void WaitForRendering();
    }
}

#endif //HINTERNAL_H
