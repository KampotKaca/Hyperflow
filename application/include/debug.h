#ifndef DEBUG_H
#define DEBUG_H

#include <hyperflow.h>

namespace app
{
    struct AppDebug
    {
        hf::FreeMoveCamera3D camera{};
        hf::GridLinesInfo gridLinesInfo{};
        bool drawGridLines = true;
    };

    extern AppDebug APP_DEBUG;

    void DebugRendererInit();
    void DebugRendererShutdown();

    void DebugDefine();
    void DebugLoad();
    void DebugStart();
    void DebugUpdate();
    void DebugQuit();
    void DebugPreRender(const hf::Ref<hf::Renderer>& rn);
    void DebugPrepass(const hf::Ref<hf::Renderer>& rn);
    void DebugRender(const hf::Ref<hf::Renderer>& rn);
}

#endif //DEBUG_H
