#include "hvk_vertbuffer.h"

namespace hf
{
    VkVertBuffer::VkVertBuffer(const VertBufferCreationInfo& info)
    {
        enableReadWrite = info.enableReadWrite;
    }

    VkVertBuffer::~VkVertBuffer()
    {

    }
}