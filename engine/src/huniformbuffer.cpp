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

        void Upload(const Ref<Renderer>& rn, const UniformBufferUploadInfo& info)
        {
            inter::HF.renderingApi.api.UploadUniformBuffer(rn->handle, info);
        }
    }

    namespace uniformstorage
    {
        UniformStorage Define(const UniformStorageDefinitionInfo& info)
        {
            return (UniformStorage)inter::HF.renderingApi.api.DefineUniformStorage(info);
        }
    }
}