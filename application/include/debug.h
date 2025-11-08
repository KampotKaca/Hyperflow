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

    void DebugPreRender();
    void DebugPrepass();
    void DebugRender();
}

#endif //DEBUG_H
