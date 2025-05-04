#include "hvk_indexbuffer.h"
#include "hvk_graphics.h"

namespace hf
{
    VkIndexBuffer::VkIndexBuffer(const IndexBufferCreationInfo& info)
    {
        indexCount = info.indexCount;
        memoryType = info.memoryType;
        indexFormat = info.indexFormat;

        switch (indexFormat)
        {
            case BufferDataType::U8:  indexType = VK_INDEX_TYPE_UINT8;  break;
            case BufferDataType::U16: indexType = VK_INDEX_TYPE_UINT16; break;
            case BufferDataType::U32: indexType = VK_INDEX_TYPE_UINT32; break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Invalid index format!!!");
        }

        uint64_t bufferSize = (uint64_t)BUFFER_DATA_SIZE[(uint32_t)info.indexFormat] * info.indexCount;

        switch (memoryType)
        {
            case BufferMemoryType::Static:
            {
                if (!info.pIndices) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                VkStaticBufferInfo createInfo
                {
                    .bufferSize = bufferSize,
                    .data = info.pIndices,
                    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                };

                CreateStaticBuffer(createInfo, &buffer, &bufferMemory);
            }
            break;
            case BufferMemoryType::WriteOnly:
            {
                VkCreateBufferInfo createInfo
                {
                    .size = bufferSize,
                    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .memoryType = BufferMemoryType::WriteOnly
                };
                CreateBuffer(createInfo, &buffer, &bufferMemory);
                if (info.pIndices) UploadBufferMemory(bufferMemory, info.pIndices, 0, bufferSize);
            }
            break;
            default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkIndexBuffer::~VkIndexBuffer()
    {
        vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffer, bufferMemory);
    }
}