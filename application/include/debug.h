#ifndef DEBUG_H
#define DEBUG_H

#include <hyperflow.h>

namespace app
{
    struct AppDebug
    {
        hf::FreeMoveCamera3D camera{};
        hf::AxisLines axisLines{};
        bool drawAxisLines = true;
    };

    extern AppDebug APP_DEBUG;

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
