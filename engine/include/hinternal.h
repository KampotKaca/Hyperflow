#ifndef HINTERNAL_H
#define HINTERNAL_H

#include "hwindow.h"
#include "hshared.h"
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
    };

    extern Hyperflow HF;

    namespace window
    {
        void Open(Window* win);
        bool Close(Window* win);

        void SetTitle(const Window* win, const char* title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);

        void SetFlags(Window* win, WindowFlags flags);
        void Focus(const Window* win);
        void Update(const Window* win);
    }

    namespace rendering
    {
        struct ShaderCreationInfo
        {
            const char* vCode;
            uint32_t vCodeSize;
            const char* fCode;
            uint32_t fCodeSize;
        };

        void Load(const char* version);
        void Unload();

        void* CreateInstance(const Window* window);
        void DestroyInstance(void* rnInstance);

        void* CreateShader(const ShaderCreationInfo& info, const Shader* shader);
        void DestroyShader(void* shader);
        bool DestroyShader_i(Shader* shader);

        void BindShader(const Renderer* renderer, const Shader* shader);

        void StartFrame(Renderer* rn);
        void EndFrame(Renderer* rn);

        void Draw(Renderer* rn);
        void WaitForRendering();
    }
}

#endif //HINTERNAL_H
