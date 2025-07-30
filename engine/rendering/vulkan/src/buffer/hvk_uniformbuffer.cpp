#include "hvk_uniformbuffer.h"
#include "hvk_graphics.h"
#include "hvk_renderer.h"

namespace hf
{
    VkUniformBuffer::VkUniformBuffer(const BufferDefinitionInfo& info) :
    VkBoundBuffer(info, BufferMemoryType::PerFrameWriteOnly, nullptr, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
    {

    }
}
