#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const BufferDefinitionInfo& info) :
        VkBufferBase(BufferMemoryType::PerFrameWriteOnly, nullptr, 0,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, ComputeBufferSize(info))
    {

    }
}
