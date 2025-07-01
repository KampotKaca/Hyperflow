#ifndef BUFFERATTRIBUTES_H
#define BUFFERATTRIBUTES_H

#include <hyperflow.h>

namespace app
{
    struct AppBufferAttributes
    {
        //Position | Normal | TextureCoord
        hf::BufferAttrib pos_nor_tex{};
    };

    extern AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll();
}

#endif //BUFFERATTRIBUTES_H
