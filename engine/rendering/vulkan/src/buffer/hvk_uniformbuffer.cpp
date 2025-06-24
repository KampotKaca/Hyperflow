#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const BufferDefinitionInfo& info) :
        VkBufferBase(info, BufferMemoryType::PerFrameWriteOnly, nullptr,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {

    }
}
