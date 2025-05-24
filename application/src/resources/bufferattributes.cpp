#include "resources/bufferattributes.h"

namespace app
{
    AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll()
    {
        APP_BUFFER_ATTRIBUTES.pctAttribute = hf::bufferattrib::Define("pos_col_tex");
    }
}