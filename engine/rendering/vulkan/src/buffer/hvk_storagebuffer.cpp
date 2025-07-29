#include "hvk_storagebuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkStorageBuffer::VkStorageBuffer(const StorageBufferDefinitionInfo& info) :
        VkBufferBase(info.memoryType, info.data, 0,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, ComputeBufferSize(info.bufferInfo))
    {

    }
}