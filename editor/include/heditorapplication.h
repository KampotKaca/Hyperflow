//
// Created by ratib on 08.11.2025.
//

#ifndef HYPERFLOW_HEDITORAPPLICATION_H
#define HYPERFLOW_HEDITORAPPLICATION_H

#include "hyperfloweditor.h"

namespace app
{
    void EditorRendererLoad();
    void EditorLoadResources();
    void EditorStart();
    void EditorPreUpdate();
    void EditorPostUpdate();
    void EditorQuit();

    void EditorRendererInit();
    void EditorRendererShutdown();

    void EditorPreRender();
    void EditorStartRender();
    void EditorEndRender();
}

#endif //HYPERFLOW_HEDITORAPPLICATION_H
