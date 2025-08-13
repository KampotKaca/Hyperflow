#include "resources/vertexattributes.h"

namespace app
{
    AppVertexAttributes APP_VERTEX_ATTRIBUTES{};

    void VertexAttributesDefineAll()
    {
        APP_VERTEX_ATTRIBUTES.instanceAttribute = hf::DefineVertexAttributeAsset("instance_default");
    }
}