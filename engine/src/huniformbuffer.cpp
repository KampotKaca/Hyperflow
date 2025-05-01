#include "hyperflow.h"
#include "hinternal.h"

namespace hf::uniformbuffer
{
    UniformBuffer Define(const UniformBufferDefinitionInfo& info)
    {
        return inter::HF.renderingApi.api.DefineUniformBuffer(info);
    }
}