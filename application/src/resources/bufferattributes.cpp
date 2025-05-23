#include "resources/bufferattributes.h"

namespace app
{
    AppBufferAttributes APP_BUFFER_ATTRIBUTES;

    void BufferAttributeDefineAll()
    {
        hf::BufferAttribFormat formats[]
        {
            { .type = hf::BufferDataType::F32, .size = 3, },
            { .type = hf::BufferDataType::F32, .size = 3, },
            { .type = hf::BufferDataType::F32, .size = 2, },
        };

        hf::BufferAttribDefinitionInfo bufferAttribDefinitionInfo
        {
            .bindingId = 0,
            .formatCount = 3,
            .pFormats = formats
        };

        APP_BUFFER_ATTRIBUTES.pctAttribute = hf::bufferattrib::Define(bufferAttribDefinitionInfo);
    }
}