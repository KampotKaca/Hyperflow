#include "resources/uniforms.h"
#include "../appconfig.h"

namespace app
{
    AppUniforms APP_UNIFORMS{};

    void UniformDefineAll()
    {

    }

    void UniformStartAll()
    {

    }

    void UniformUploadAll(const hf::Ref<hf::Renderer>& rn)
    {
        const hf::UniformBufferUpload uniformUpload
        {
            .buffer = hf::primitives::GetGlobalUniformBuffer(),
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(GlobalUniformInfo),
            .data = &APP_UNIFORMS.globalUniformInfo
        };

        rn->Upload_Uniform(uniformUpload);
    }
}