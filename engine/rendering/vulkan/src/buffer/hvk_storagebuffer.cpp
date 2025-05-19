#include "hvk_storagebuffer.h"
#include "hvk_graphics.h"

namespace hf
{
    VkStorageBuffer::VkStorageBuffer(const StorageBufferCreationInfo& info)
        : bindingId(info.bindingId), bufferSize(info.elementCount * info.elementSizeInBytes), memoryType(info.memoryType)
    {
        switch (memoryType)
        {
        case BufferMemoryType::Static:
            {
                if (!info.data) throw GENERIC_EXCEPT("[Hyperflow]", "Static buffer is without any data, this is not allowed");

                VkStaticBufferInfo createInfo
                {
                    .bufferSize = bufferSize,
                    .data = info.data,
                    .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (uint32_t)info.usageFlags,
                };

                CreateStaticBuffer(createInfo, &buffer, &bufferMemory);
            }
            break;
        case BufferMemoryType::WriteOnly:
            {
                VkCreateBufferInfo createInfo
                {
                    .size = bufferSize,
                    .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (uint32_t)info.usageFlags,
                    .memoryType = BufferMemoryType::WriteOnly,
                    .pQueueFamilies = nullptr,
                    .familyCount = 0,
                };

                CreateBuffer(createInfo, &buffer, &bufferMemory);
                VK_HANDLE_EXCEPT(vmaMapMemory(GRAPHICS_DATA.allocator, bufferMemory, &mapping));
                if (info.data) UploadBufferMemory((const void*)info.data, mapping, 0, bufferSize);
            }
            break;
        default: throw GENERIC_EXCEPT("[Hyperflow]", "Unknown memory type");
        }
    }

    VkStorageBuffer::~VkStorageBuffer()
    {
        if (mapping)
        {
            vmaUnmapMemory(GRAPHICS_DATA.allocator, bufferMemory);
            mapping = nullptr;
        }
        vmaDestroyBuffer(GRAPHICS_DATA.allocator, buffer, bufferMemory);
    }
}