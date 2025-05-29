#ifndef BUFFERATTRIBUTES_H
#define BUFFERATTRIBUTES_H

#include <hyperflow.h>

namespace app
{
    struct AppBufferAttributes
    {
        //Position | Color | TextureCoord
        hf::BufferAttrib pos_col_tex{};
    };

    extern AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll();
}

#endif //BUFFERATTRIBUTES_H
