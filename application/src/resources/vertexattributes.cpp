#include "resources/vertexattributes.h"

namespace app
{
    AppVertexAttributes APP_VERTEX_ATTRIBUTES{};

    void VertexAttributesDefineAll()
    {
        APP_VERTEX_ATTRIBUTES.defaultInstanceAttribute = hf::DefineVertexAttributeAsset("instance_default");
        APP_VERTEX_ATTRIBUTES.litInstanceAttribute = hf::DefineVertexAttributeAsset("lit_default");
    }
}