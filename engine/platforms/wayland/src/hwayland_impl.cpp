#include "hplatform.h"
#include "hinternal.h"

namespace hf::platform
{
    void Load(){}
    void Unload(){}

    ivec2 GetPointerPosition(const Window* window){ return {}; }
    void HandleEvents(EngineUpdateType updateType){}
    uint32_t CreateVulkanSurface(void* windowHandle, void* instance, void* surfaceResult){ return 0; }

    namespace window
    {
        void Open(Window* win){}
        bool Close(Window* win){ return true; }

        void SetTitle(const Window* win, const std::string& title){}
        void SetSize(const Window* win, ivec2 size){}
        void SetPosition(const Window* win, ivec2 position){}
        void SetRect(const Window* win, IRect rect){}

        void SetFlags(Window* win, WindowFlags flags){}
        void Focus(const Window* win){}
    }
}
