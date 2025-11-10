#ifndef HYPERFLOW_VERTEXATTRIBUTES_H
#define HYPERFLOW_VERTEXATTRIBUTES_H

#include <hyperflow.h>

namespace app
{
    struct AppVertexAttributes
    {
        hf::VertexBufferAttribute defaultInstanceAttribute{};
        hf::VertexBufferAttribute litInstanceAttribute{};
    };

    extern AppVertexAttributes APP_VERTEX_ATTRIBUTES;

    void VertexAttributesDefineAll();
}

#endif //HYPERFLOW_VERTEXATTRIBUTES_H
