#include "hvk_storagebuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkStorageBuffer::VkStorageBuffer(const StorageBufferDefinitionInfo& info) :
    VkBoundBuffer(info.bufferInfo, info.memoryType, info.data, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
    {

    }
}