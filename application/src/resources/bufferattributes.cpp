#include "resources/bufferattributes.h"

namespace app
{
    AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll()
    {
        APP_BUFFER_ATTRIBUTES.pos_col_tex = hf::DefineBufferAttrib("pos_col_tex");
    }
}