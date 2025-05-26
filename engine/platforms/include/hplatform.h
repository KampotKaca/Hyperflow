#ifndef HPLATFORM_H
#define HPLATFORM_H

#include "hshared.h"

namespace hf::platform
{
    void Load();
    void Unload();
    void Sleep(double seconds);

    void HandleEvents(EngineUpdateType updateType);

    void* LoadDll(const char* dllName);
    void UnloadDll(void* dll);
    void* GetFuncPtr(void* dll, const char* funcName);

    RenderingApiType GetBestRenderingApi();
    bool IsValidRenderingApi(RenderingApiType api);
}

#endif //HPLATFORM_H
