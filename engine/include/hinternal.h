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
        std::string appTitle;
        Time time;
        Ref<Window> mainWindow;
        std::vector<Ref<Window>> windows;
    };

    extern Hyperflow HF;

    namespace window
    {
        void SetTitle(const Window* win, const char* title);
        void SetSize(const Window* win, ivec2 size);
        void SetPosition(const Window* win, ivec2 position);
        void SetRect(const Window* win, IRect rect);

        void SetFlags(Window* win, WindowFlags flags);
        void Focus(const Window* win);
        bool Close(Window* win);
        void Update(const Window* win);
    }

    namespace rendering
    {
        void StartFrame(Renderer* renderer);
        void EndFrame(Renderer* renderer);
    }
}

#endif //HINTERNAL_H
