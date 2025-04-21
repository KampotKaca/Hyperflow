#include "include/hvk_vertbuffer.h"
#include "hyperflow.h"
#include "hinternal.h"

#include "hgenericexception.h"
#include "../config.h"

namespace hf::inter::rendering
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

    void* CreateVertBuffer(const VertBufferCreationInfo& info)
    {
        return new VkVertBuffer(info);
    }

    void DestroyVertBuffer(void* handle)
    {
        delete (VkVertBuffer*)handle;
    }
}