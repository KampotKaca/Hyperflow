#include "resources/bufferattributes.h"

namespace app
{
    AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll()
    {
        APP_BUFFER_ATTRIBUTES.pos_nor_tex = hf::DefineBufferAttrib("pos_nor_tex");
    }
}