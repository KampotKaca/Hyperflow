#include "haxislines.h"
#include "hyperflow.h"
#include "hinternal.h"

namespace hf
{
    void AxisLines::GetNecessaryUniforms(UniformBuffer* location)
    {
        location[0] = inter::HF.staticResources.axisLineUniform;
    }

    void AxisLines::Draw(const Ref<Renderer>& rn) const
    {
        static inter::AxisLineUniform uploadUniform{};
        uploadUniform.planeNormal = vec4(planeNormal, lineThickness);
        uploadUniform.color = color;

        UniformBufferUpload upload
        {
            .buffer = inter::HF.staticResources.axisLineUniform,
            .offsetInBytes = 0,
            .sizeInBytes = sizeof(inter::AxisLineUniform),
            .data = &uploadUniform
        };

        UniformBufferUploadInfo uploadInfo
        {
            .bindingType = RenderBindingType::Graphics,
            .setBindingIndex = 1,
            .pUploads = &upload,
            .uploadCount = 1
        };

        uniformbuffer::Upload(rn, uploadInfo);

        DrawCallInfo info
        {
            .pVertBuffers = &inter::HF.staticResources.quadBuffer,
            .bufferCount = 1,
            .indexBuffer = nullptr,
            .instanceCount = 1
        };

        renderer::Draw(rn, info);
    }
}