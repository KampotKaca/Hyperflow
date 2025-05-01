#include "hyperflow.h"
#include "hinternal.h"

namespace hf::bufferattrib
{
    BufferAttrib Define(const BufferAttribDefinitionInfo& info)
    {
        uint32_t fullStride = 0;
        for (uint32_t i = 0; i < info.formatCount; i++)
        {
            auto& stride = info.pFormats[i];
            stride.lSize = stride.size * BUFFER_DATA_SIZE[(uint32_t)stride.type];
            fullStride += stride.lSize;
        }

        return inter::HF.renderingApi.api.DefineVertBufferAttrib(info, fullStride);
    }
}