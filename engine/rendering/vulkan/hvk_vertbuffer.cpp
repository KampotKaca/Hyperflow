#include "include/hvk_vertbuffer.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
    {
        this->dataSize = dataSize;
        enableReadWrite = info.enableReadWrite;

        if (enableReadWrite)
        {
            data = utils::Allocate(dataSize);
            memcpy(data, info.vertices, dataSize);
        }
    }

    VkVertBuffer::~VkVertBuffer()
    {

    }
}