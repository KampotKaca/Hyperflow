#ifndef HHYPERFLOWEDITOR_H
#define HHYPERFLOWEDITOR_H

#include "hshared.h"

namespace hf
{
    struct EditorInfo
    {
        void* windowHandle{};
        void* renderApiHandles{};
        MultisampleMode multisampleMode = MultisampleMode::MSAA_1X;
    };

    void LoadEditor(const EditorInfo& info);
    void UnloadEditor();

    void EditorBeginFrame();
    void EditorEndFrame();
}

#endif //HHYPERFLOWEDITOR_H
