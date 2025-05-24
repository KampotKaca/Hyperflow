#ifndef DEBUG_H
#define DEBUG_H

#include <hyperflow.h>

namespace app
{
    void DebugDefine();
    void DebugLoad();
    void DebugStart();
    void DebugUpdate();
    void DebugQuit();
    void DebugPreRender(const hf::Ref<hf::Renderer>& rn);
    void DebugRender(const hf::Ref<hf::Renderer>& rn);
}

#endif //DEBUG_H
