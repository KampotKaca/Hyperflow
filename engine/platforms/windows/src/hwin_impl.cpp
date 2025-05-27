#include "hwin_shared.h"
#include "hyperflow.h"

namespace hf::platform
{
    void Load()
    {
        timeBeginPeriod(1);
    }

    void Unload()
    {
        timeEndPeriod(1);
    }

    void HandleEvents(EngineUpdateType updateType) {}

    RenderingApiType GetBestRenderingApi()
    {
        return RenderingApiType::Direct3D;
    }

    bool IsValidRenderingApi(RenderingApiType api)
    {
        switch (api)
        {
        case RenderingApiType::Vulkan:
        case RenderingApiType::Direct3D: return true;
        default: return false;
        }
    }
}