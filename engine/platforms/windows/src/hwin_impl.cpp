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

    size_t GetMemoryPageSize()
    {
        // https://learn.microsoft.com/en-gb/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
        SYSTEM_INFO system_info;
        GetSystemInfo(&system_info);
        return system_info.dwPageSize; // TYPICALLY 4096, 2^ 12
    }

    size_t GetMemoryPageAllocationGranularity() { return 65536; }
}