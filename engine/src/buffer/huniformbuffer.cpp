#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    namespace uniformbuffer
    {
        UniformBuffer Define(const UniformBufferDefinitionInfo& info)
        {
            return (UniformBuffer)inter::HF.renderingApi.api.DefineUniformBuffer(info);
        }
    }

    namespace uniformallocator
    {
        UniformAllocator Define(const UniformAllocatorDefinitionInfo& info)
        {
            return (UniformAllocator)inter::HF.renderingApi.api.DefineUniformAllocator(info);
        }
    }
}